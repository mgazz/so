#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>


typedef struct 
{
	pthread_cond_t condizione1; 
	pthread_cond_t condizione2; 
	pthread_mutex_t mut;
}monitor;

/*static sem_t visitatore_arrivato; //globale*/
/*static sem_t biglietteri_libera; //globale*/
/*static sem_t device_disponibili; //globale*/


monitor m; 

static sem_t condizione1;
static sem_t condizione2;
pthread_mutex_t sem_mutex;



void* user1_sem(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n",id);
	for (int i = 0; 1; ++i)
	{
		printf("[%d] faccio semwait\n",id);
		sem_wait (&condizione1);
		printf("passato %d volte\n",i+1);
	}
	pthread_exit(NULL);
	
}




int main(int argc, char *argv[])
{
	pthread_t th1;
	//inizializzazione mod monitor

	//inizializzazione mod semafori
	sem_init(&condizione1,0,15);
	pthread_mutex_init(&sem_mutex,NULL);
	
	//creo il thread 
	printf("creazione users\n");
	pthread_create(&th1,NULL,user1_sem, (void *)1);
	//aspetto i thread
	pthread_join(th1,NULL);

	printf("main fine\n");
	
	return 0;
}
