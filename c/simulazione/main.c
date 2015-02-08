#include "main.h"

#define N 5 
#define MAX 2 

static sem_t primo_piatto_su_bancone;
static sem_t secondo_piatto_su_bancone;
static sem_t finiti_primi_piatti;
static sem_t bancone_non_pieno;

pthread_mutex_t mut;

int n_primi_piatti_finiti;
int piatti_su_bancone;
int n_secondi_piatti_finiti;


void init()
{
	pthread_mutex_init(&mut,NULL);
	sem_init(&primo_piatto_su_bancone,0,0);
	sem_init(&secondo_piatto_su_bancone,0,0);
	sem_init(&finiti_primi_piatti,0,0);
	sem_init(&bancone_non_pieno,0,0);
	n_primi_piatti_finiti=0;
	piatti_su_bancone=0;
	
	n_secondi_piatti_finiti=0;
	
}

void print_status()
{
	printf("\t\t------status------\n");
	printf("\t\tprimi_piatti_finiti: %d\n",n_primi_piatti_finiti); //print_status;
	printf("\t\tpiatti_su_bancone: %d\n",piatti_su_bancone); 
	printf("\t\tn_secondi_piatti_finiti: %d\n",n_secondi_piatti_finiti); 
	printf("\t\t------------------\n");
}



void* cuoco(void* arg)
{
	int id = (int)arg;
	for (int i = 0; i < N; ++i)
	{
		if (piatti_su_bancone == MAX)
		{
			printf("cuoco [%d] aspetto il ritiro di un piatto per andare a vanti\n",id);
			sem_wait (&bancone_non_pieno);
		}
		pthread_mutex_lock(&mut);
		piatti_su_bancone++;
		print_status();
		pthread_mutex_unlock(&mut);
		sem_post (&primo_piatto_su_bancone);
	}
	sem_wait (&finiti_primi_piatti);
	printf("cuoco [%d] tutti hanno terminato i primi piatti --> procedo con i secondi\n",id);
	for (int i = 0; i < N; ++i)
	{
		if (piatti_su_bancone == MAX)
		{
			printf("cuoco [%d] aspetto il ritiro di un piatto per andare a vanti\n",id);
			sem_wait (&bancone_non_pieno);
		}
		pthread_mutex_lock(&mut);
		piatti_su_bancone++;
		print_status();
		pthread_mutex_unlock(&mut);
		sem_post (&secondo_piatto_su_bancone);
	}
	
	pthread_exit(NULL);
}

void* dipendente(void* arg)
{
	int id = (int)arg;
	sem_wait (&primo_piatto_su_bancone);
	pthread_mutex_lock(&mut);
	piatti_su_bancone--;
	printf("dipendente [%d] preso primo piatto\n",id);
	if (piatti_su_bancone==MAX-1)
	{
		sem_post (&bancone_non_pieno);
	}
	pthread_mutex_unlock(&mut);

	sleep(rand()%5);
	pthread_mutex_lock(&mut);
	n_primi_piatti_finiti++;
	printf("dipendente [%d] terminato primo piatto\n",id);
	if (n_primi_piatti_finiti==N) //tutti hanno finito dico a cuoco di procedere
	{
		sem_post (&finiti_primi_piatti);
	}
	pthread_mutex_unlock(&mut);

	sem_wait (&secondo_piatto_su_bancone);

	pthread_mutex_lock(&mut);
	piatti_su_bancone--;
	printf("dipendente [%d] preso second piatto\n",id);
	if (piatti_su_bancone==MAX-1)
	{
		sem_post (&bancone_non_pieno);
	}
	pthread_mutex_unlock(&mut);

	sleep(rand()%5);

	pthread_mutex_lock(&mut);
	n_secondi_piatti_finiti++;
	printf("dipendente [%d] terminato secondo piatto\n",id);
	pthread_mutex_unlock(&mut);
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	init();
	print_status();
	pthread_t dipendenti[N];
	pthread_t cuoc;
	
	//creazione cuoco
	if(pthread_create(&cuoc,NULL,cuoco,(void *)0)<0)
	{
		fprintf(stderr, "errore creazione thread cuoco\n");
		exit(1);
	}
	
	
	//creazione dipendenti
	for (int i = 0; i < N; ++i)
	{
		if(pthread_create(&dipendenti[i],NULL,dipendente,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread dipendente [%d]\n",i);
			exit(1);
		}
	}
	
	//wait dipendenti
	for (int i = 0; i < N; ++i)
	{
		if(pthread_join(dipendenti[i],NULL)){
			fprintf(stderr, "errore terminazione thread dipendente [%d]\n",i);
			exit(1);
		}
	}

	//wait cuoco
	if(pthread_join(cuoc,NULL)){
		fprintf(stderr, "errore terminazione thread cuoco [%d]\n");
		exit(1);
	}
	
	return 0;
}
