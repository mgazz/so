#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define NUM_THREADS 4
void *Calcolo(void *t) // codice worker
{ 
	int i;
	long tid, result=0;
	tid = (int)t;
	printf("Thread %ld è partito...\n",tid);
	for (i=0; i<100; i++)
		result = result + tid;
	printf("Thread %ld ha finito. Ris= %ld\n",tid, result);
	pthread_exit((void*) result);
}

int main (int argc, char *argv[])
{ 
	pthread_t thread[NUM_THREADS];
	int rc;
	long t;
	long status;
	for(t=0; t<NUM_THREADS; t++) {
		printf("Main: creazione thread %ld\n", t);
		rc=pthread_create(&thread[t], NULL, Calcolo, (void *)t);
		if (rc) {
		 printf("ERRORE: %d\n", rc);
		 exit(-1); 
		}
	}
	for(t=0; t<NUM_THREADS; t++) {
		rc = pthread_join(thread[t], (void *)&status);
		if (rc)
		printf("ERRORE join thread %ld codice %d\n", t, rc);
		else
		printf("Finito thread %ld con ris. %ld\n",t,status);
	}
} 
