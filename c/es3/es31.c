#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define MAX 10 //CAPACITA' DEL PONTE
#define EN_N 0 //ENTRA NORD
#define ES_N 1 //ENTRA SUD
#define EN_S 2 //ESCI NORD
#define ES_S 3 //ESCI SUD

#define O 0   //ORDINARI
#define E 1   //ESPERTI

#define NORD 0
#define SUD 1

#define K 3

#define NUM_T 10 //NUMERO THREAD

typedef struct
{
	pthread_mutex_t lock;
	pthread_cond_t coda[4][2];
	int sospesi[4][2];
	int sulponte[2];
} monitor;

monitor m;

void init_monitor(monitor* m)
{
	int i = 0;
	pthread_mutex_init(&m->lock, NULL);
	for (i = 0; i<4; i++){
	pthread_cond_init(&m->coda[i][O], NULL);
	pthread_cond_init(&m->coda[i][E], NULL);
	m->sospesi[i][O] = 0;
	m->sospesi[i][E] = 0;
	}
	m->sulponte[NORD] = 0;
	m->sulponte[SUD] = 0;
}

void entraNORD(monitor* m, int id,int tipo)
{
	pthread_mutex_lock (&m->lock);

	//while necessario perche bisogna ritestare la condizione?
	while ( m->sulponte[NORD] + m->sulponte[SUD] == MAX || 
			(tipo == 0 && m->sospesi[EN_N][E]>0) || 
			(abs(m->sulponte[NORD]-m->sulponte[SUD])==K &&
			 m->sulponte[NORD] > m->sulponte[SUD]))
	{
		printf("Mezzo %d sospeso di tipo %d in ingresso della corsia NORD!\n", id,tipo);
		m->sospesi[EN_N][tipo]++;
		pthread_cond_wait(&m->coda[EN_N][tipo], &m->lock);
		m->sospesi[EN_N][tipo]--;
	}

	printf("Mezzo %d di tipo %d DENTRO la corsia NORD!\n", id,tipo);
	m->sulponte[NORD]++;
	if (m->sulponte[NORD] < m->sulponte[SUD])
	{
		if (m->sospesi[ES_N][E]>0) // segnalo Emerg in uscita Nord
			pthread_cond_signal(&m->coda[ES_N][E]);
		else if (m->sospesi[ES_N][O]>0)  // segnalo Ordinario in uscita Nord
			pthread_cond_signal(&m->coda[ES_N][O]);
		else if (m->sospesi[EN_S][E]>0) // segnalo emergenza in ingresso Sud
			pthread_cond_signal(&m->coda[EN_S][E]);
		else if (m->sospesi[EN_S][O]>0) // segnalo Ordinario in ingresso Sud
			pthread_cond_signal(&m->coda[EN_S][O]);

	}
	pthread_mutex_unlock (&m->lock);
}

void entraSUD(monitor* m, int id,int tipo)
{
	pthread_mutex_lock (&m->lock);

	while ( 
			m->sulponte[NORD] + m->sulponte[SUD] >= MAX || 
			(tipo == 0 && m->sospesi[EN_S][E]>0) ||
	(abs(m->sulponte[NORD]-m->sulponte[SUD])==K && m->sulponte[NORD] < m->sulponte[SUD]))
	{
		printf("Mezzo %d sospeso di tipo %d in ingresso della corsia SUD!\n", id,tipo);
		m->sospesi[EN_S][tipo]++;
		pthread_cond_wait(&m->coda[EN_S][tipo], &m->lock);
		m->sospesi[EN_S][tipo]--;
	}

	printf("Mezzo %d di tipo %d DENTRO la corsia SUD!\n", id,tipo);
	m->sulponte[SUD]++;
	if (m->sulponte[NORD] > m->sulponte[SUD])
	{
		if (m->sospesi[ES_S][E]>0) // segnalo Emerg in uscita Sud
					pthread_cond_signal(&m->coda[ES_S][E]);
		else if (m->sospesi[ES_S][O]>0)  // segnalo Ordinario in uscita Sud
					pthread_cond_signal(&m->coda[ES_S][O]);
		else if (m->sospesi[EN_N][E]>0) // segnalo emergenza in ingresso Nord
					pthread_cond_signal(&m->coda[EN_N][E]);
		else if (m->sospesi[EN_N][O]>0) // segnalo Ordinario in ingresso Nord
					pthread_cond_signal(&m->coda[EN_N][O]);

	}
	pthread_mutex_unlock (&m->lock);
}

void esceNORD(monitor* m, int id,int tipo) //ESCE DALLA CORSIA NORD
{
	pthread_mutex_lock (&m->lock);
	while (
			abs(m->sulponte[NORD]-m->sulponte[SUD])==K && 
			m->sulponte[NORD] < m->sulponte[SUD])
	{
		printf("Mezzo %d di tipo %d proveniente da nord sospeso in uscita!\n", id, tipo);
		m->sospesi[ES_N][tipo]++;
		pthread_cond_wait(&m->coda[ES_N][tipo], &m->lock);
		m->sospesi[ES_N][tipo]--;
	}
	printf("Mezzo %d di tipo %d ESCE dalla corsia NORD!\n", id, tipo);
	m->sulponte[NORD]--;
	if (m->sospesi[EN_N][E] > 0)
		pthread_cond_signal(&m->coda[EN_N][E]);
	else if (m->sospesi[EN_N][O] > 0)
		pthread_cond_signal(&m->coda[EN_N][O]);
	else if (m->sulponte[NORD] > m->sulponte[SUD])
		{
			if (m->sospesi[ES_S][E]>0) // segnalo Emerg in uscita Sud
						pthread_cond_signal(&m->coda[ES_S][E]);
			else if (m->sospesi[ES_S][O]>0)  // segnalo Ordinario in uscita Sud
						pthread_cond_signal(&m->coda[ES_S][O]);
		}
	pthread_mutex_unlock (&m->lock);
}



void esceSUD(monitor* m, int id,int tipo) //ESCE DALLA CORSIA SUD
{
	pthread_mutex_lock (&m->lock);
	while (
			abs(m->sulponte[NORD]-m->sulponte[SUD])==K && 
			m->sulponte[NORD] > m->sulponte[SUD])
	{
		printf("Mezzo %d di tipo %d proveniente da nord sospeso in uscita!\n", id, tipo);
		m->sospesi[ES_S][tipo]++;
		pthread_cond_wait(&m->coda[ES_S][tipo], &m->lock);
		m->sospesi[ES_S][tipo]--;
	}
	printf("Mezzo %d di tipo %d ESCE dalla corsia SUD!\n", id, tipo);
	m->sulponte[SUD]--;

	if (m->sospesi[EN_S][E] > 0)
		pthread_cond_signal(&m->coda[EN_S][E]);

	else if (m->sospesi[EN_S][O] > 0)
		pthread_cond_signal(&m->coda[EN_S][O]);

	else if (m->sulponte[NORD] < m->sulponte[SUD])
	{
		if (m->sospesi[ES_N][E]>0) // segnalo Emerg in uscita Nord
			pthread_cond_signal(&m->coda[ES_N][E]);
		else if (m->sospesi[ES_N][O]>0)  // segnalo Ordinario in uscita Nord
			pthread_cond_signal(&m->coda[ES_N][O]);
	}
	pthread_mutex_unlock (&m->lock);
}

void* pNORD(void* arg)
{
	int id = (int)arg;
	int tipo = rand() % 2;
	printf("Mezzo %d di tipo %d NORD!\n", id, tipo);
	entraNORD(&m, id,tipo);
	sleep(1);
	esceNORD(&m, id,tipo);
	pthread_exit(NULL);
}

void* pSUD(void* arg)
{
	int id = (int)arg;
	int tipo = rand() % 2;
	printf("Mezzo %d di tipo %d SUD!\n", id, tipo);
	entraSUD(&m, id,tipo);
	sleep(1);
	esceSUD(&m, id,tipo);
	pthread_exit(NULL);
}

int main (int argc, char** argv)
{
	int i, tipo;
	pthread_t thread[NUM_T];

	init_monitor(&m);

	srand(time(NULL));

	for(i = 0; i < NUM_T; i++)
	{
		tipo = rand() % 2;
		if (tipo)
			pthread_create(&thread[i], NULL, pNORD, (void*)i);
		else
			pthread_create(&thread[i], NULL, pSUD, (void*)i);
	}

	for(i = 0; i < NUM_T; i++)
		pthread_join(thread[i], NULL);

	return 0;
}



