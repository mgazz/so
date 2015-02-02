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

void* user1(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n",id);

	pthread_mutex_lock(&m.mut);
	printf("[%d] aspetto condizione1\n",id);
	pthread_cond_wait(&m.condizione1,&m.mut);
	printf("[%d] sbloccata condizione1\n",id);
	printf("[%d] sblocco condizione2\n",id);
	pthread_cond_signal(&m.condizione2);
	pthread_mutex_unlock(&m.mut);

	pthread_exit(NULL);
	
}

void* user2(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n",id);

	pthread_mutex_lock(&m.mut);
	printf("[%d] sblocco condizione1\n",id);
	pthread_cond_signal(&m.condizione1);
	printf("[%d] aspetto condizione2\n",id);
	pthread_cond_wait(&m.condizione2,&m.mut);
	printf("[%d] sbloccata condizione2\n",id);
	pthread_mutex_unlock(&m.mut);

	pthread_exit(NULL);
	
}


void* user1_sem(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n",id);
	printf("[%d] aspetto condizione1\n",id);
	sem_wait (&condizione1);
	printf("[%d] sbloccata condizione1\n",id);
	printf("[%d] sblocco condizione2\n",id);
	sem_post (&condizione2);
	pthread_mutex_unlock(&m.mut);
	pthread_exit(NULL);
	
}

void* user2_sem(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n",id);

	
	printf("[%d] sblocco condizione1\n",id);
	sem_post (&condizione1);
	printf("[%d] aspetto condizione2\n",id);
	sem_wait (&condizione2);
	printf("[%d] sbloccata condizione2\n",id);

	pthread_exit(NULL);
	
}

void init()
{
	printf("----- inizializzazione monitor ------\n");

	pthread_mutex_init(&m.mut,NULL);

	//inizializzazione della condizione
	pthread_cond_init(&m.condizione1,NULL);
	pthread_cond_init(&m.condizione2,NULL);

	printf("---------------------------------------\n");
}


int main(int argc, char *argv[])
{
	int i;

	pthread_t th1,th2;
	//inizializzazione mod monitor
	init(&m);

	//inizializzazione mod semafori
	sem_init(&condizione1,0,0);
	sem_init(&condizione2,0,0);
	pthread_mutex_init(&sem_mutex,NULL);
	
	//creo il thread 
	printf("creazione users\n");
	pthread_create(&th1,NULL,user1, (void *)1);
	pthread_create(&th2,NULL,user2, (void *)2);
	pthread_join(th1,NULL);
	pthread_join(th2,NULL);
	printf("---------------------------------------\n");
	pthread_create(&th1,NULL,user1_sem, (void *)1);
	pthread_create(&th2,NULL,user2_sem, (void *)2);
	//aspetto i thread
	pthread_join(th1,NULL);
	pthread_join(th2,NULL);

	printf("main fine\n");
	
	
	
	return 0;
}
