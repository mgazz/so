#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 10
#define NDEV 10
#define NGRUPPO 10
#define MAX 20

typedef struct monitor
{
	int id_utenti_in_coda_biglietteri[N];
	int n_utenti_in_coda_biglietteria;
	int n_device_disponibili;
	int n_utenti_in_gruppo;

	bool v_a;
	bool b_l;

	pthread_cond_t visitatore_arrivato; 
	pthread_cond_t biglietteria_libera; 

	pthread_mutex_t mut_biglietteria;
};

/*static sem_t visitatore_arrivato; //globale*/
/*static sem_t biglietteri_libera; //globale*/
/*static sem_t device_disponibili; //globale*/


monitor m; 

void visitore(void* arg)
{
	int id = (int)arg;
	printf("visitatore [%d] partito \n",id);

	//passo per la biglietteria
	pthread_mutex_lock(&m.mut_biglietteria);
	while(b_l==false) {
		printf("visitatore [%d] aspetta che la biglietteria si liberi\n");
		&m.n_utenti_in_coda_biglietteria++;
		pthread_cond_wait(&m.biglietteri_libera,&m.mut);
	}
	printf("visitatore [%d] servito dalla biglietteria\n");
	b_l=false;
	&m.n_utenti_in_coda_biglietteria--;
	pthread_mutex_unlock(&m.mut_biglietteria);
	printf("visitatore [%d] esce dalla biglietteria\n");
	
}

void biglietteria()
{
	printf("biglietteria partito \n",id);
	while(exe) {
		//se non disponibili dispositivi o gruppo pieno aspetto
		pthread_mutex_lock(&m->mut_biglietteria);
		&m.b_l=true;
		pthread_mutex_unlock(&m->mut_biglietteria);
		while(&m.n_dev==0 || 
					&m.n_utenti_in_gruppo==NGRUPPO) {
			pthread_cond_wait(&m.libera_inserimento_in_gruppo);
			
		}
	}
	pthread_mutex_unlock(&m->mut);
}

void init(monitor* m)
{
	printf("----- inizializzazione monitor ------\n");
	m->n_utenti_in_coda_device=0;
	m->n_utenti_in_coda_biglietteria=0;
	//la biglietteria sceglie quando aprire
	m->b_l=false;

	//inizializzazione del mutex
	pthread_mutex_init(&m->mut_biglietteria,NULL);

	//inizializzazione della condizione
	pthread_cond_init(&m->mut_biglietteria,NULL);

	printf("---------------------------------------\n");
}

int main(int argc, char *argv[])
{
	int i;
	int retval[N];

	pthread_t th_utenti[N];
	pthread_t th_biglietteria;
	
	//creo il thread della biglietteria
	pthread_create(th_biglietteria,NULL,biglietteria,NULL);
	// creo gli utenti
	pthread_create(&t_utenti[i],NULL,visita,(void *)i);

	for (i = 0; i < N; ++i)
	{
		if(pthread_create(&t_utenti[i],NULL,visita,(void *)i)<0)
		{
			fprintf(stderr, "thread [%d] creazione: error%d\n",i);
			exit(1);
		}
	}

	
	for (i = 0; i < N; ++i)
	{
		if(pthread_join(utenti[i],(void *)&retval[i])){
			fprintf(stderr, "thread [%d] join: error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo, retval=[%d]\n", i,retval[i]);
		}
	}

	pthread_join(th_museo,NULL);
	printf("main fine\n");
	
	
	
	return 0;
}
