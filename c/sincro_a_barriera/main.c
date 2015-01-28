#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//dominio risposta 1-10
//presentare risultati parziali:
//ogni k domande stampato voto medio del film

//termine sondaggio risultati definitivi
//voto medio ciascun film
//film max punteggio

//campione persone/thread
#define N 15

//numero di film e domande
//ogni film K domane
#define K 2

//array[film][domanda]
int sondaggio[K][K];

//accesso mutualmente esclusivo
pthread_mutex_t m;

static sem_t s;
static sem_t b;


int pers_int;

int risp[N][K][K];
int res[N][K][K];
int total[K][K];

void fill_data()
{
	int i,j,z;
	for (i = 0; i < N; ++i)
	{
		printf("persona [%d]:\n", i);
		for (j = 0; j < K; ++j)
		{
			printf("\tfilm [%d]:", j);
			for (z = 0; z < K; ++z)
			{
				risp[i][j][z] = rand()%10;
				printf("[%d]", risp[i][j][z]);
			}
				printf("\n");
		}
		
	}
}

void print_result()
{
	int i,j,z;
	/*for (j = 0; j < K; ++j)*/
	/*{*/
		/*printf("film [%d] - tot: %d \n",j, total[j]);*/
	/*}*/
	printf("======stampa parziale =======\n");
	for (i = 0; i < pers_int; ++i)
	{
		printf("persona [%d]:\n", i);
		for (j = 0; j < K; ++j)
		{
			printf("\tfilm [%d]:", j);
			for (z = 0; z < K; ++z)
			{
				printf("[%d]", risp[i][j][z]);
			}
				printf("\n");
		}
		
	}
	for (j = 0; j < K; ++j)
	{
		printf("film [%d]:\n", j);
		for (z = 0; z < K; ++z)
		{
			printf("\ttotal domanda %d: [%.2f]\n",z+1,(float)total[j][z]/pers_int);
		}
		printf("\n");
	}
	printf("============================\n");
	
}

void responde(void *arg)
{
	int id = (int)arg;
	int j,z;
	pthread_mutex_lock(&m);

	printf("th[%d]-sono la [%d] pers_int\n",id,pers_int);

	//inserimento total qui
	for (j = 0; j < K; ++j)
	{
		for (z = 0; z < K; ++z)
		{
			res[id][j][z] = risp[pers_int][j][z];
			// per ogni film aggiungo il valore inserito
			total[j][z] = total [j][z] + risp[pers_int][j][z];
			printf("[%d]", res[pers_int][j][z]);
		}
		printf("\n");
	}

	//autmento il valore su cui ciclare
	pers_int++;
	print_result();

	pthread_mutex_unlock(&m);
	if (pers_int == N)
	{
		sem_post (&s);
	}
	//finito di compilare aspetto di guardare il film
	sem_wait (&b);
	printf("th [%d] ha guardato il film\n", id);
	sem_post (&b);
	
	
	pthread_exit((void *) id);
}

int main(int argc, char *argv[])
{
	fill_data();
	int retval[N];
	int i,j,z;
	int cur_film;
	int max;
	int k;
	pers_int = 0;
	pthread_t th[N];
	printf("------ start -------\n");

	// semaforo parte bloccato
	sem_init(&s,0,0);
	sem_init(&b,0,0);

	for (i = 0; i < N; ++i)
	{
		if(pthread_create(&th[i],NULL,responde,(void *)i)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
	}
	//finche tutte pers non intervistate aspetto
	sem_wait (&s);
	printf("sondaggi finiti\n");
	/*k = 1;*/
	/*while(k) {*/
		/*if (pers_int = n)*/
		/*{*/
			/*//se tutt persone intervistate stampo*/
			/*k=0;*/
		/*}*/
	/*}*/

	max=0;
	for (j = 0; j < K; ++j)
	{
		cur_film=0;
		for (z = 0; z < K; ++z)
		{
			// ogni film totale delle risposte di tutti gli utenti
			cur_film += total[j][z];
		}
		
		// media sulle risposte di tutti gli utenti sul singolo film
		printf("media film [%d]: [%.2f]\n", j,(float)cur_film/(K*N));

		if (cur_film/K > max)
		{
			max = j;
		}
	}
	printf("best film: %d\n", max);
	// faccio partire la visione
	sem_post (&b);

	for (i = 0; i < N; ++i)
	{
		if(pthread_join(th[i],(void *)&retval[i])){
			fprintf(stderr, "thread [%d] terminato con error \n",(int) retval[i]);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo\n", i);
		}
	}
	
	return 0;
}
