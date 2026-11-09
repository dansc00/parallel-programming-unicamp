/*
 * sum_scalar.c - A simple parallel sum program to sum a
 * series of scalars
 */

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAXTHREADS 8

void *sum(void *p);

// add padding between each partial sum 
typedef struct{
	unsigned long int value;
	char padding[64 - sizeof(unsigned long int)]; // 64 bytes(size of a cache line)
} padded_psum;

// global shared variables
padded_psum psum[MAXTHREADS]; // partial sum computed by each thread
unsigned long int sumtotal = 0; // total sum
unsigned long int n; // input
int numthreads; // number of threads
pthread_mutex_t mutex; 
//--------------------------------------------------
void *sum(void *p) {
int myid = *((int *)p);
unsigned long int start = (myid * (unsigned long int)n) / numthreads;
unsigned long int end = ((myid + 1) * (unsigned long int)n) / numthreads;
unsigned long int i;

for (i = start; i < end; i++) {
	psum[myid].value += 2;
}

pthread_mutex_lock(&mutex);
sumtotal += psum[myid].value;
pthread_mutex_unlock(&mutex);

return NULL;
}

int main(int argc, char **argv) {
	pthread_t tid[MAXTHREADS]; // threads
	int i, myid[MAXTHREADS]; // threads IDs
	struct timeval start, end;

	gettimeofday(&start, NULL); /* start timing */

	if (argc != 3) {
		printf("Usage: %s <n> <numthreads>\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = strtoul(argv[1], NULL, 10); // convert str to unsigned long int (str, pointer_to_rest, base)
	numthreads = (int)strtoul(argv[2], NULL, 10);

	for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		psum[i].value = 0.0;
		pthread_create(&tid[i], NULL, sum, &myid[i]);
	}

	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	gettimeofday(&end, NULL); /* end timing */
	long spent = (end.tv_sec * 1000000 + end.tv_usec) -
							(start.tv_sec * 1000000 + start.tv_usec);

	printf("sum = %lu\ntime_us = %ld\n", sumtotal, spent);

	return EXIT_SUCCESS;
}

 
 