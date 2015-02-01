
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	int utenti;
	pthread_cond_t coda[N]; 
	int ncoda[N]; 
	pthread_mutex_t mut;
} monitor;

monitor m; 

void init(monitor* m)
{
	int i;
	printf("----- inizializzazione monitor ------\n");
	
	m->utenti=0;
	printf("m->utenti: %d\n", m->utenti);	
	for (i = 0; i < N; ++i)
	{
		pthread_cond_init(&m->coda[i],NULL);
		m->ncoda[i]=0;
		printf("ncoda[ %d ][ %d ][ %d ]: %d \n",i,m->ncoda[i]);
	}
	pthread_mutex_init(&m->mut,NULL);
	printf("---------------------------------------\n");
}
