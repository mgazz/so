#include "main.h"
#include <signal.h>

#define MAXT 5 

static sem_t cond_stop;

void* cliente(void* arg)
{
	int id = (int)arg;
	printf("cliente [%d] creato\n",id);
	while(1) {
		sem_wait (&cond_stop);
		sleep(3);
		printf("cliente [%d] scrive\n",id);
	}
}

void kill()
{
	printf("segnalato termine\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t clienti[MAXT];
	sem_init(&cond_stop,0,0);
	
	signal(SIGUSR1,kill);//prima di pthread_create
	
	//creazione clienti
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&clienti[i],NULL,cliente,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread cliente [%d]\n",i);
			exit(1);
		}
	}
	sleep(10);
	printf("termino e me ne fotto\n");
	return 0;
}
