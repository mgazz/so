#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct monitor
{
	pthread_cond_t condizione1; 
	pthread_cond_t condizione2; 
	pthread_mutex_t mt;
};

/*static sem_t visitatore_arrivato; //globale*/
/*static sem_t biglietteri_libera; //globale*/
/*static sem_t device_disponibili; //globale*/


monitor m; 

void user1(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n");
	pthread_mutex_lock(&m.mut);
	while(1) {
		printf("%d|libero condizione1\n",id);
		pthread_cond_signal(&m.condizione1);
		printf("%d|aspetto condizione2\n",id);
		pthread_cond_wait(&m.condizione2,&m.mut);
		printf("%d|liberata condizione 1\n",id);
	}
	pthread_mutex_unlock(&m.mut);
	
}

void user2(void* arg)
{
	int id = (int)arg;
	printf("user [%d] start\n");
	pthread_mutex_lock(&m.mut);
	while(1) {
		printf("%d|aspetto condizione1\n",id);
		pthread_cond_wait(&m.condizione1,&m.mut);
		printf("%d|liberata condizione 1\n",id);
		pthread_cond_signal(&m.condizione2);
		printf("%d|libero condizione2\n",id);
	}
	pthread_mutex_unlock(&m.mut);
	
}


void init(monitor* m)
{
	printf("----- inizializzazione monitor ------\n");

	pthread_mutex_init(&m->mut,NULL);

	//inizializzazione della condizione
	pthread_cond_init(&m->condizione1,NULL);
	pthread_cond_init(&m->condizione2,NULL);

	printf("---------------------------------------\n");
}

int main(int argc, char *argv[])
{
	int i;
	int retval[N];

	pthread_t th[2];
	init(&m);
	
	//creo il thread 
	for (i = 0; i < 2; ++i)
	{
		if(pthread_create(&th[i],NULL,visita,(void *)i)<0)
		{
			fprintf(stderr, "thread [%d] creazione: error%d\n",i);
			exit(1);
		}
	}
	
	//aspetto i thread
	for (i = 0; i < 2; ++i)
	{
		if(pthread_join(utenti[i],(void *)&retval[i])){
			fprintf(stderr, "thread [%d] join: error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo, retval=[%d]\n", i,retval[i]);
		}
	}

	printf("main fine\n");
	
	
	
	return 0;
}
