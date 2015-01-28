/*
Nel periodo natalizio una grande azienda dolciaria produce una cesta regalo composta da N specialità dell’azienda (torroni, cioccolatini, caramelle, ecc.).
Le ceste vengono confezionate una alla volta .
In particolare la preparazione di ogni articolo da inserire nella cesta è affidata a un processo distinto Pi (i=1,.. N), che si occupa della produzione dell’articolo;
una volta pronto, l’articolo viene depositato nella cesta.
Il processo PC che si occupa del confezionamento della cesta deve quindi aspettare che tutti gli N processi produttori Pi abbiano effettuato il deposito per poter 
procedere al confezionamento; teminato il confezionamento, il processo PC può procedere alla realizzazione di una nuova cesta.
Ogni processo Pi, prima di depositare un nuovo articolo deve aspettare che sia terminato il confezionamento della cesta precedente.
Si realizzi un’applicazione concorrente in c/pthread che rappresenti il processo PC e i processi Pi (i=1,..N) mediante thread concorrenti e che rispetti i vincoli dati mediante un’opportuna politica di sincronizzazione realizzata tramite semafori.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <semaphore.h>

#define N 3
#define THREAD N+1

sem_t preparare[N];
sem_t preparati[N];
int cesti_pronti;

void * produttore(void *arg)
{
    int id;
    id = *((int*) arg);
		printf("[%d]-id\n",id);
		
    while(1)
    {
        sem_wait(&preparare[id]);
        printf("[produttore %d] aggiunto articolo:%d\n",id,id);
        sem_post(&preparati[id]);
        sleep(1);
    }
    return NULL;
}
void * confezionatore(void *arg)
{
    int id, i;
    id = *((int*) arg);
		printf("[%d]-id\n",id);
    while(1)
    {
        //attesa completamento articoli
        for(i=0;i<N;i++)
            sem_wait(&preparati[i]);

        cesti_pronti++;
        printf("[confezionatore] finita cesta n. %d\n", cesti_pronti);
        sleep(3);
        
        //riattivazione produttori
        for(i=0;i<N;i++)
            sem_post(&preparare[i]);

        sleep(2);
    }
    
    return NULL;
}
void init()
{
	int i;
	for (i = 0; i < N; ++i)
	{
		sem_init(&preparati[i],0,0);
		sem_init(&preparare[i],0,1);
	}
}
int main(int argc, char *argv[])
{
	int i;
	int ids[THREAD];
	pthread_t thread[THREAD];
	i=0;
	init();
	for (i = 0; i < N; ++i)
	{
		ids[i]=i;
		if(pthread_create(&thread[i],NULL,produttore,&ids[i])<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
		
	}

	ids[N]=i;
	if(pthread_create(&thread[i],NULL,confezionatore,&i)<0)
	{
		fprintf(stderr, "pthread_create error for thread 1\n");
		exit(1);
	}

  sleep(THREAD*3);

	for (i = 0; i < THREAD; ++i)
	{
		pthread_join(thread[i],NULL);
	}
	
	return 0;
}
