#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <gperftools/profiler.h>

#define SEED 123

void free_matrix(int **m, int size) {
  for (int i = 0; i < size; i++)
    free(m[i]);
  free(m);
}

int **mul(int **a, int **b, int size) {
  int **ret = malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++) {
    ret[i] = calloc(size, sizeof(int));
    for (int j = 0; j < size; j++)
      for (int k = 0; k < size; k++)
        ret[i][j] += a[i][k] * b[k][j];
  }

  free_matrix(a, size);
  free_matrix(b, size);

  return ret;
}

// Parallelize this function:
int **array_mul(int ***data, int n, int size) {

  int ***ret = data;
  int count = n;
  
  #pragma omp parallel
  #pragma omp single
  {
  while(count > 1){
    int count_aux = (count + 1)/2;
    int ***ret_aux = malloc(count_aux * sizeof(int**));

    for(int i = 0; i < count_aux; i++){
      #pragma omp task firstprivate(i, count) shared(ret)
      {
        if(2*i+1 < count)
          ret_aux[i] = mul(ret[2*i], ret[2*i+1], size);
        else
          ret_aux[i] = ret[2*i];
      }
    }
    #pragma omp taskwait
    
    ret = ret_aux;
    count = count_aux;
    }
  }
  
  int **result = ret[0];
  free(ret);
  return result;
}

int **rnd_matrix(int size) {
  int **ret = malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++) {
    ret[i] = malloc(size * sizeof(int));
    for (int j = 0; j < size; j++)
      ret[i][j] = 2 * (rand() % 2) - 1; // Generates -1 or 1
  }

  return ret;
}

void print_matrix(int **m, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++)
      printf("%d ", m[i][j]);
    printf("\n");
  }
}

int main(int argc, char **argv) {
  int n, size;
  double t;
  FILE *input;

  //ProfilerStart("../gperftools/parallel.prof");
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file!\n");
    return EXIT_FAILURE;
  }

  if ((input = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: could not open input file!\n");
    return EXIT_FAILURE;
  }

  fscanf(input, "%d %d", &n, &size);
  srand(SEED);

  // Do not change this line
  omp_set_num_threads(4);

  int ***data = malloc(n * sizeof(int **));
  for (int i = 0; i < n; i++)
    data[i] = rnd_matrix(size);

  t = omp_get_wtime();
  int **ret = array_mul(data, n, size);
  t = omp_get_wtime() - t;

  print_matrix(ret, size);
  fprintf(stderr, "%lf\n", t);

  free_matrix(ret, size);
  free(data);
  //ProfilerStop();
  return 0;
}
