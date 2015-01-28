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
#define N 4

//numero di film e domande
//ogni film K domane
#define K 2

//array[film][domanda]
int sondaggio[K][K];

//accesso mutualmente esclusivo
pthread_mutex_t m;

int pers_int;

int risp[N][K][K];
int res[N][K][K];
int media[K];

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
	for (j = 0; j < K; ++j)
	{
		printf("film [%d] - tot: %d \n",j, media[j]);
	}
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
				media[j] +=risp[i][j][z];
			}
				printf("\n");
		}
		
	}
	for (j = 0; j < K; ++j)
	{
		printf("film [%d] - tot: %d \n",j, media[j]);
	}
	for (j = 0; j < K; ++j)
	{
		printf("film [%d] - media: %d \n",j, media[j]/(K*pers_int));
	}
	printf("============================\n");
	
}

void responde(void *arg)
{
	int id = (int)arg;
	int j,z;
	pthread_mutex_lock(&m);

	printf("th[%d]-sono la [%d] pers_int\n",id,pers_int);

	//inserimento media qui
	for (j = 0; j < K; ++j)
	{
		for (z = 0; z < K; ++z)
		{
			res[id][j][z] = risp[pers_int][j][z];
			printf("[%d]", res[pers_int][j][z]);
		}
		printf("\n");
	}

	//autmento il valore su cui ciclare
	pers_int++;
	print_result();

	pthread_mutex_unlock(&m);
	pthread_exit((void *) id);
}

int main(int argc, char *argv[])
{
	fill_data();
	int retval[N];
	int i;
	pers_int = 0;
	pthread_t th[N];
	printf("------ start -------\n");
	for (i = 0; i < N; ++i)
	{
		if(pthread_create(&th[i],NULL,responde,(void *)i)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
	}
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
