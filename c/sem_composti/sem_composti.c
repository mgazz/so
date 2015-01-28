#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 13

//semafori per imporre l'ordine di accesso (P1,P2,P3)
//alla variabile V
static sem_t s1,s2,s3;
int V=0,F=0;

void *thread1_process(void * arg)
{
	
	int k=1;
	while(k) {
		sem_wait(&s1);
		if (V<MAX)
		{
			V++;
		}
		else {
			k=0;
			printf("[%d]-T1: %d (V=%d)\n",getpid(),++F,V);
			
		}
		sem_post(&s2);
	}
	pthread_exit(0);
}

void *thread2_process(void * arg)
{
	
	int k=1;
	while(k) {
		//aspetto il mio semaforo: 2
		sem_wait(&s2);
		if (V<MAX)
		{
			V++;
		}
		else {
			k=0;
			printf("[%d]-T2: %d (V=%d)\n",getpid(),++F,V);
			
		}
		//quando ho finito libero il semaforo dopo: 3
		sem_post(&s3);
	}
	pthread_exit(0);
}

void *thread3_process(void * arg)
{
	
	int k=1;
	while(k) {
		//aspetto il mio semaforo: 3
		sem_wait(&s3);
		if (V<MAX)
		{
			V++;
		}
		else {
			k=0;
			printf("[%d]-T3: %d (V=%d)\n",getpid(),++F,V);
			
		}
		//quando ho finito libero il semaforo dopo: 3
		sem_post(&s1);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t th1,th2,th3;

	//2par: 0 se il semaforo non condiviso
	//3par: valore iniziale
	sem_init(&s1,0,1);
	sem_init(&s2,0,0);
	sem_init(&s3,0,0);
	if(pthread_create(&th1,NULL,thread1_process,NULL)<0)
	{
		fprintf(stderr, "pthread_create error for thread 1\n");
		exit(1);
	}
	if(pthread_create(&th2,NULL,thread2_process,NULL)<0)
	{
		fprintf(stderr, "pthread_create error for thread 1\n");
		exit(1);
	}
	if(pthread_create(&th3,NULL,thread3_process,NULL)<0)
	{
		fprintf(stderr, "pthread_create error for thread 1\n");
		exit(1);
	}

	pthread_join(th1,NULL);
	pthread_join(th2,NULL);
	pthread_join(th3,NULL);

	return 0;
}
