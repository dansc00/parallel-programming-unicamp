#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define COMMENT "Histogram_GPU"
#define RGB_COMPONENT_COLOR 255 // 1 byte = 8 bits = 0-255 dec

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void check_cuda(cudaError_t error, const char *filename, const int line)
{
  if (error != cudaSuccess) {
    fprintf(stderr, "Error: %s:%d: %s: %s\n", filename, line,
                 cudaGetErrorName(error), cudaGetErrorString(error));
    exit(EXIT_FAILURE);
  }
}

#define CUDACHECK(cmd) check_cuda(cmd, __FILE__, __LINE__)

typedef struct {
  unsigned char red, green, blue;
} PPMPixel;

typedef struct {
  int x, y; // width, height
  PPMPixel *data; // pixel array
} PPMImage;

static PPMImage *readPPM(const char *filename) {
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;
  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  if (!fgets(buff, sizeof(buff), fp)) {
    perror(filename);
    exit(1);
  }

  if (buff[0] != 'P' || buff[1] != '6') {
    fprintf(stderr, "Invalid image format (must be 'P6')\n");
    exit(1);
  }

  img = (PPMImage *)malloc(sizeof(PPMImage));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  c = getc(fp);
  while (c == '#') {
    while (getc(fp) != '\n')
      ;
    c = getc(fp);
  }

  ungetc(c, fp);
  if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
    exit(1);
  }

  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
    fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
    exit(1);
  }

  if (rgb_comp_color != RGB_COMPONENT_COLOR) {
    fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
    exit(1);
  }

  while (fgetc(fp) != '\n')
    ;
  img->data = (PPMPixel *)malloc(img->x * img->y * sizeof(PPMPixel));

  if (!img->data) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
    fprintf(stderr, "Error loading image '%s'\n", filename);
    exit(1);
  }

  fclose(fp);
  return img;
}

__global__ void histogram_kernel(PPMPixel *data, float *h, int n) {

    int i = blockIdx.x * blockDim.x + threadIdx.x; // thread index
    int x; // histogram index

    while (i < n){

      data[i].red = ((data[i].red * 4) / 256);
      data[i].green = ((data[i].green * 4) / 256);
      data[i].blue = ((data[i].blue * 4) / 256);

      x = 16*data[i].red + 4*data[i].green + data[i].blue; 

      atomicAdd(&h[x], 1); 
     
      i += blockDim.x * gridDim.x; // jump to next iteration
    }
  }

double Histogram(PPMImage *image, float *h) {
  float ms;
  cudaEvent_t start, stop;
  int n = image->y * image->x; // size of pixels array

  // device variables
  PPMPixel *d_data; // pixels matrix
  float *d_h; // histogram

  // device memory allocate
  CUDACHECK(cudaMalloc((void **) &d_data, n * sizeof(PPMPixel)));
  CUDACHECK(cudaMalloc((void **) &d_h, sizeof(float) * 64));

  // cuda copy host to device
  CUDACHECK(cudaMemcpy(d_data, image->data, n * sizeof(PPMPixel), cudaMemcpyHostToDevice));
  CUDACHECK(cudaMemcpy(d_h, h, sizeof(float) * 64, cudaMemcpyHostToDevice));

  // Create Events
  CUDACHECK(cudaEventCreate(&start));
  CUDACHECK(cudaEventCreate(&stop));

   // define number of blocks based on number of threads
   int threads = 512;
   int blocks = (n + threads - 1)/threads;

  // Launch kernel and compute kernel runtime.
  // Warning: make sure only the kernel is being profiled, memcpies should be
  // out of this region.
  CUDACHECK(cudaEventRecord(start));
  histogram_kernel<<<blocks, threads>>>(d_data, d_h, n);
  CUDACHECK(cudaDeviceSynchronize());
  CUDACHECK(cudaEventRecord(stop));
  CUDACHECK(cudaEventSynchronize(stop));
  CUDACHECK(cudaEventElapsedTime(&ms, start, stop));

  // Destroy events
  CUDACHECK(cudaEventDestroy(start));
  CUDACHECK(cudaEventDestroy(stop));

  // cuda copy device to host
  CUDACHECK(cudaMemcpy(h, d_h, sizeof(float) * 64, cudaMemcpyDeviceToHost));

  CUDACHECK(cudaFree(d_data));
  CUDACHECK(cudaFree(d_h));

  return ((double)ms) / 1000.0;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return 1;
  }

  PPMImage *image = readPPM(argv[1]);
  float *h = (float *)malloc(sizeof(float) * 64);

  int i;
  // Initialize histogram
  for (i = 0; i < 64; i++)
    h[i] = 0.0;

  // Compute histogram
  double t = Histogram(image, h);

  // normalize histogram
  for(i = 0; i < 64; i++)
    h[i] = h[i]/(image->y * image->x);

  for (int i = 0; i < 64; i++)
    printf("%0.3f ", h[i]);
  printf("\n");

  fprintf(stderr, "%lf\n", t);
  free(h);
}
