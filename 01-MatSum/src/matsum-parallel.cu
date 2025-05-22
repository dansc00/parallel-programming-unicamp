#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

__global__ void matrix_sum(int *A, int *B, int *C, int n) {
  // TODO: Implement this kernel!
  int i = blockIdx.x * blockDim.x + threadIdx.x; // thread index

  while (i < n){
      C[i] = A[i] + B[i];
      i += blockDim.x * gridDim.x; // jump to next iteration
  }
}

int main(int argc, char **argv) {
  int *A, *B, *C;
  int *d_A, *d_B, *d_C; // device variables 
  int i, j;
  double t;
  int bytes, threads, blocks, n; 

  // Input
  int rows, cols;
  FILE *input;

  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return EXIT_FAILURE;
  }

  if ((input = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return EXIT_FAILURE;
  }

  fscanf(input, "%d", &rows);
  fscanf(input, "%d", &cols);

  n = rows * cols;
  bytes = sizeof(int) * n;

  // Allocate memory on the host
  A = (int *)malloc(bytes);
  B = (int *)malloc(bytes);
  C = (int *)malloc(bytes);

  // Allocate memory on the device
  cudaMalloc((void **) &d_A, bytes);
  cudaMalloc((void **) &d_B, bytes);
  cudaMalloc((void **) &d_C, bytes);

  // Initialize memory
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      A[i * cols + j] = B[i * cols + j] = i + j;
    }
  }

  // Copy data to device
  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice);

  // define number of blocks based on number of threads
  threads = 512;
  blocks = (n + threads - 1)/threads;

  // Compute matrix sum on device
  // Leave only the kernel and synchronize inside the timing region!
  t = omp_get_wtime();
  matrix_sum<<<blocks, threads>>>(d_A, d_B, d_C, n);
  cudaDeviceSynchronize();
  t = omp_get_wtime() - t;

  // Copy data back to host
  cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost);

  long long int sum = 0;

  // Keep this computation on the CPU
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      sum += C[i * cols + j];
    }
  }

  fprintf(stdout, "%lli\n", sum);
  fprintf(stderr, "%lf\n", t);

  free(A);
  free(B);
  free(C);
  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
}
