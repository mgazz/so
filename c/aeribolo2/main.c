/*in questo caso l'addetto che fa solo dei post non aspetta l'arrivo di tutti i passeggeri*/
#include "main.h"
static sem_t arrivo_passeggero_check;
static sem_t libero_check;
static sem_t kit_diponibile;
static sem_t accesso_aereo_libero;
static sem_t fine_accesso_passeggero;



pthread_mutex_t mut;
int ykit=0;
int nkit=0;
int kit_in_cesta=0;
int serviti=0;

void* passeggero(void* arg)
{
	int id = (int)arg;
	int tipo;
	/*printf("passeggero %d creato\n",id);*/
	sem_wait (&libero_check);
	sleep(rand()%5);
	sem_post (&arrivo_passeggero_check);
	printf("passeggero %d passato checkin\n",id);
	tipo=rand()%2;//scelta tra prendere il kit o no
	if (tipo==0)//prendo il kit
	{
		/*pthread_mutex_lock(&mut)*/
		/*[>ykit++;<]*/
		/*pthread_mutex_unlock(&mut)*/
		/*printf("passeggero %d aspetta kit\n",id);*/
		sem_wait (&kit_diponibile);
		/*sleep(rand()%5);*/
		pthread_mutex_lock(&mut);
		serviti++;
		kit_in_cesta--;
		printf("passeggero %d preso kit\n",id);
		pthread_mutex_unlock(&mut);
	}
	else {//non prendo il kit
		pthread_mutex_lock(&mut);
		/*nkit++;*/
		serviti++;
		pthread_mutex_unlock(&mut);
		/*printf("passeggero %d non prende il kit e va diretto all'imbarco\n",id);*/
	}
	sem_wait (&accesso_aereo_libero);
	sleep(rand()%2);
	sem_post (&fine_accesso_passeggero);
	
	
	
	pthread_exit(NULL);
}

void* addetto(void* arg)
{
	int id = (int)arg;
	printf("addetto %d creato\n",id);
	for (int i = 0; i < N; ++i)
	{
		/*printf("addetto %d libero il check\n",id);*/
		sem_post (&libero_check);
		sem_wait (&arrivo_passeggero_check);
	}
	printf("addetto %d tutti i passeggeri passato checkin\n",id);
	printf("addetto %d carico il cesto\n",id);
	while(1) {
		pthread_mutex_lock(&mut);
		/*if (nkit+ykit<N || serviti<N)*/
		if (serviti<N)
		{
			if (kit_in_cesta<MAX_C)
			{
				pthread_mutex_unlock(&mut);
				sleep(rand()%2);
				pthread_mutex_lock(&mut);
				kit_in_cesta++;
				printf("addetto %d inserito kit in cesta \n",id);
				pthread_mutex_unlock(&mut);
				sem_post (&kit_diponibile);
			}
			else {
				//cesta piena
				/*printf("addetto %d cesta piena\n",id);*/
				pthread_mutex_unlock(&mut);
			}
		}
		else {
			//tutti i passeggeri preso kit
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	printf("addetto %d tutti i passeggeri passato stage kit\n",id);
	printf("------addetto %d apro le porte per l'imbarco------\n",id);
	for (int i = 0; i < N; ++i)
	{
		sem_post (&accesso_aereo_libero);
		sem_wait (&fine_accesso_passeggero);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

	pthread_t passeggeri[N];
	pthread_t addett;
	
	sem_init(&arrivo_passeggero_check,0,0);
	sem_init(&libero_check,0,0);
	sem_init(&kit_diponibile,0,0);
	sem_init(&fine_accesso_passeggero,0,0);
	sem_init(&accesso_aereo_libero,0,0);
	pthread_mutex_init(&mut,NULL);

	//creazione addetto
	if(pthread_create(&addett,NULL,addetto,(void *)0)<0)
	{
		fprintf(stderr, "errore creazione thread addetto\n");
		exit(1);
	}

	//creazione passeggeri
	for (int i = 0; i < N; ++i)
	{
		if(pthread_create(&passeggeri[i],NULL,passeggero,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread passeggero [%d]\n",i);
			exit(1);
		}
	}

	//wait passeggeri
	for (int i = 0; i < N; ++i)
	{
		if(pthread_join(passeggeri[i],NULL)){
			fprintf(stderr, "errore terminazione thread passeggero [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread passeggero [%d] termiato con successo\n", i);
		}
	}
	
	//wait addetto
	if(pthread_join(addett,NULL)){
		fprintf(stderr, "errore terminazione thread addetto %d\n");
		exit(1);
	}
	else {
		printf("pthread addetto termiato con successo\n");
	}
	
	
	printf("----- fine------\n");
}
