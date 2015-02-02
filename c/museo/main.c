#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <museo.h>

#define MAX 10 //porta fuori
#define NDEV 10
#define DIM_GRUPPO 5

typedef struct museo
{
	sem_t visitatore_in_biblietteria;  
	sem_t out_bigl;  
	sem_t start_vis;
	sem_t in_consegna_dev;
	sem_t out_consegna_dev; //necessario?
	sem_t servito;  

	pthread_mutex_t mut;
	pthread_mutex_cond dev_disp;

	bool visitando; // dice se stanno visitando o no
	int n_vis_gruppo
	int n_vis_coda;
	int n_dev_disp;;

};


void init(museo* m)
{
	sem_init(&m->visitatore_in_biblietteria,0,1);
	sem_init(&m->in_gruppo_vis,0,0);
	sem_init(&m->start_vis,0,0);
	sem_init(&m->in_consegna_dev,0,1);
	sem_init(&m->out_consegna_dev,0,0);
	sem_init(&m->servito,0,0);

	pthread_mutex_init(&m->mut,NULL)

	pthread_cond_init(&m.dev_disp,NULL)

	n_vis_coda=0;
	n_dev_disp=NDEV
}
static sem_t s; //porta fuori

pthread_mutex_t m;

bool exe = true; //esecuzione



void visitatore(void* arg)
{
	int id = (int)arg;
	printf("thread [%d] partito \n",id);
	sem_wait (&m.visitatore_in_biblietteria);
	printf("visitatore [%d] esce dalla biglietteria e aspetta il device\n",id);
	sem_wait(&m.visitatore_aspetta_device);
	printf("visitatore [%d] reperito device ed entra nel gruppo\n",id);
	pthread_mutex_lock(&m);
	//sezione critica
	
	pthread_mutex_unlock(&m);
}

void th_museo(void* arg)
{
	int id = (int)arg;
	printf("thread [%d] partito \n",id);

	//contina fino a stop importo da main
	//1 ciclo corrisponde all'arrivo di N
	while(exe) {
		for (int i = 0; i < ; ++i)
		{
			sem_wait (&m.visitatore_in_biblietteria);
			printf("museo | visitatore arrivato\n");

			pthread_mutex_lock(&m.mut)
			m.n_vis_gruppo++
			pthread_mutex_unlock(&m.mut)
			sleep(1)
			sem_post (&m.out_bigl);
			printf("museo | visitatori serviti [%d]\n",m.n_vis_gruppo);
		}
		printf("museo | visitatori [%d] pronti per visita --> start_visita\n");

		//qui non dovrebbe partire la visita?
	}
	
}

museo m;
int main(int argc, char *argv[])
{
	sem_init(&s,0,0);
	
	int i;
	int ret_val[MAX];
	pthread_t utenti[MAX];

	//creazione dei visitatori
	for (i = 0; i < MAX; ++i)
	{
		if(pthread_create(&th_utente[i],NULL,visitatore,(void *)i)<0)
		{
			fprintf(stderr, "thread [%d] creazione: error%d\n",i);
			exit(1);
		}
	}

	//creazione del museo
	if (pthread_create(&th_museo,NULL,museo,NULL))
	{
		fprintf(stderr, "museo creazione thread\n",);
		exit(1);
		
	}
	
	//attesa degli utenti
	for (i = 0; i < MAX; ++i)
	{
		if(pthread_join(utenti[i],(void *)&ret_val[i])){
			fprintf(stderr, "thread [%d] join: error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo, ret_val=[%d]\n", i,ret_val[i]);
		}
	}

	printf("stoppo il museo\n");
	exe = false;

	//attesa del museo
	if(pthread_join(th_museo,NULL))
	{
		fprintf(stderr, "museo join: error \n");
		exit(1);
	}
	else {
		printf("museo terminato con successo\n");
	}
	
	return 0;
}
