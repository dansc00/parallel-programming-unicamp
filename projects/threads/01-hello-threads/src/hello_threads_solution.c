#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Print hello world from a thread
void *printHello(void *threadId) {
  long tid = *((long*)threadId);

  printf("Hello Worlds from thread #%ld!\n", tid);

  // Pause for 3 seconds
  sleep(3);

  printf("Goodbye from thread #%ld!\n", tid);

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) { // verify if num_threads is passed via command line
    printf("Usage: %s <num_threads>\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]); // char to int convertion
  long t; // thread ID

  // allocates threads array
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

  // allocates threads ID array
  long *tids = malloc(num_threads * sizeof(long));

  for (t = 0; t < num_threads; t++) {
    printf("Creating thread #%ld\n", t);

    tids[t] = t;
    if(pthread_create(&threads[t], NULL, printHello, (void*)&tids[t]) != 0){
      perror("thread creation error");
      exit(EXIT_FAILURE);
    }
  }

  // Wait for all threads to finish
  for (t = 0; t < num_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  free(threads);
  free(tids);

  return EXIT_SUCCESS;
}