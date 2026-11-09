#define _GNU_SOURCE
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

const int max_password_value = 500000;
const char cmd_format[300] = "unzip -P%d -t %s 2>&1";

FILE *popen(const char *command, const char *type);

int main(int argc, char **argv) {

  FILE *input;
  char filename[100];
  double t;
  int nt;

  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return 1;
  }

  if ((input = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return 1;
  }

  // Read inputs
  fscanf(input, "%d", &nt);
  fscanf(input, "%s", filename);

  // Do not touch this line
  omp_set_num_threads(nt);

  t = omp_get_wtime();

  int password_found = 0;

  #pragma omp parallel shared(password_found) 
  #pragma omp single
  {
    for (int i = 0; i < max_password_value && !password_found; i++) {

      #pragma omp task firstprivate(i) shared(filename, cmd_format, password_found)
      {
        // local variables declaration, avoid race condition
        FILE *fp = NULL;
        char cmd[400];
        char ret[200];

        sprintf((char*)&cmd, cmd_format, i, filename);
        fp = popen(cmd, "r");

        while (!feof(fp) && !password_found) {
          fgets((char*)&ret, sizeof(ret), fp);
      
          if (strcasestr(ret, "ok")) {
            printf("Password: %d\n", i);

            #pragma omp atomic write
            password_found = 1;
          }
        }
        pclose(fp);
      }
    }
  }

  t = omp_get_wtime() - t;
  fprintf(stderr, "%lf\n", t);

  return 0;
}
