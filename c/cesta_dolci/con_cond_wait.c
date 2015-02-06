 //NB: processo PC chiamato cesta
 //NB: processo Pi chiamati produttore
#include "main.h"

static sem_t cesta_libera_per_inserimento;
static sem_t inserimento_del_produttore;
pthread_mutex_t mut;
pthread_cond_t avvio;
int ceste_fatte=0;
int prod_in_cesta=0;


void* produttore(void* arg)
{
	int id = (int)arg;
	/*printf("produttore [%d] creato\n",id);*/
	while(1) {
		pthread_mutex_lock(&mut);
		pthread_cond_wait(&avvio,&mut);
		if (ceste_fatte<CS) //condizione di stop
		{
			pthread_mutex_unlock(&mut);
			sem_wait (&cesta_libera_per_inserimento);
			sleep(rand()%5);

			pthread_mutex_lock(&mut);
			prod_in_cesta++;
			printf("produttore [%d] inserita spec in cesta\n",id);
			pthread_mutex_unlock(&mut);

			sem_post (&inserimento_del_produttore);
		}
		else {
			//condizione stop verificata
			printf("produttore [%d] termina\n",id);
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	pthread_exit(NULL);
}
void* cesta(void* arg)
{
	int id = (int)arg;
	printf("cesta [%d] creato\n",id);
	while(1) {
		pthread_mutex_lock(&mut);
		printf("cesta [%d] start:%d\n",id);
		pthread_cond_broadcast(&avvio);
		if (ceste_fatte<CS)
		{
			pthread_mutex_unlock(&mut);
			//do
			for (int i = 0; i < N; ++i)
			{
				sem_post(&cesta_libera_per_inserimento);
				sem_wait(&inserimento_del_produttore);
			}
			printf("cesta [%d] tutte le spec inserite nella cesta\n",id);
			sleep(rand()%5); //confezionamento
			pthread_mutex_lock(&mut);
			ceste_fatte++;
			prod_in_cesta=0;
			pthread_mutex_unlock(&mut);
		}
		else {
			//condizione verificata
			printf("cesta [%d] termina\n",id);
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t produttori[N];
	pthread_t pc; // thread che rappresenta la cesta
	sem_init(&inserimento_del_produttore,0,0);
	sem_init(&cesta_libera_per_inserimento,0,0);
	pthread_mutex_init(&mut,NULL);
	pthread_cond_init(&avvio_confezione,NULL);

	//creazione cesta
	if(pthread_create(&pc,NULL,cesta,(void *)0)<0)
	{
		fprintf(stderr, "errore creazione thread cesta\n");
		exit(1);
	}
	
	//creazione produttori
	for (int i = 0; i < N; ++i)
	{
		if(pthread_create(&produttori[i],NULL,produttore,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread produttore [%d]\n",i);
			exit(1);
		}
	}
	
	//wait produttori
	for (int i = 0; i < N; ++i)
	{
		if(pthread_join(produttori[i],NULL)){
			fprintf(stderr, "errore terminazione thread produttore [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread produttore [%d] termiato con successo\n", i);
		}
	}

	//wait cesta
	if(pthread_join(pc,NULL)){
		fprintf(stderr, "errore terminazione thread cesta [%d]\n");
		exit(1);
	}
	else {
		printf("pthread cesta termiato con successo\n");
	}

	printf("----- fine------\n");
}
