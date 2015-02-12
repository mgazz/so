#include "main.h"

#define MAXT 6 //6 processi 
#define N 1 //6 processi 
#define Pp 0 //porche
#define Pf 1 //ferrari
#define Pl 2 //lambo
#define P1 0 //carrozzeria
#define P2 1 //montaggio ruote

static sem_t avvio_processo[3][2];
static sem_t avvio_confezione; //avvio confezione se 3 contenitori non vuoti
static sem_t contenitore_non_pieno[3];

pthread_mutex_t mut;
int contenitore[3];
int nconfezioni=3;
int lap=0;



void init() //inizializzazione dei monitor
{
	sem_init(&avvio_confezione,0,0);
	pthread_mutex_init(&mut,NULL);
	for (int i = 0; i < 3; ++i)
	{
		sem_init(&contenitore_non_pieno[i],0,0);
		contenitore[3]=0; //inizializzazione contenitore
		for (int j = 0; j < 2; ++j)
		{
			if (j==P1)
			{
				printf("avvio processo %d %d libero\n",i,j);
				sem_init(&avvio_processo[i][P1],0,1);
			} else if(j==P2){
				sem_init(&avvio_processo[i][P2],0,0); //processi su macchina 1 alla volta
			}
		}
	}
}

void print_status	()
{
	printf("nconfezioni_fatte: %d\n",lap);//print_status;
	for (int i = 0; i < 3; ++i)
	{
		printf("contenitore[%d]: %d\n",i,contenitore[i]); //print_status;
	}
}

void* processo(void* arg)
{
	int id = (int)arg;
	int tipo_macchina;
	tipo_macchina= id%3;
	int tipo_processo;
	tipo_processo= id%2;
	printf("processo [%d] %d %d \n",id,tipo_processo,tipo_macchina);
	while(1) {
		if (lap < nconfezioni) //variabile utilizzata per il controllo del numero di confezioni da effettuare
		{
			if (tipo_processo==P1) 
			{
				sem_wait (&avvio_processo[tipo_macchina][P1]);
				/*sleep(rand()%5);*/
				printf("processo [%d] %d %d terminato -> faccio partire P2\n",id,tipo_processo,tipo_macchina);
				sem_post (&avvio_processo[tipo_macchina][P2]);
			} else if (tipo_processo==P2) {
				sem_wait (&avvio_processo[tipo_macchina][P2]);
				printf("processo [%d] %d %d P2 partito\n",id,tipo_processo,tipo_macchina);
				/*sleep(rand()%5);*/
				/*sem_post(&avvio_processo[tipo_macchina][P1]);*/
				pthread_mutex_lock(&mut);
				print_status();
				if (contenitore[tipo_macchina]==N)
				{
					printf("processo [%d] %d %d contenitore pieno: %d \n",id,tipo_processo,tipo_macchina,contenitore[tipo_macchina]);
					sem_wait(&contenitore_non_pieno[tipo_macchina]);
					printf("processo [%d] %d %d risvegliato: %d \n",id,tipo_processo,tipo_macchina,contenitore[tipo_macchina]);
				}
				contenitore[tipo_macchina]++;
				printf("processo [%d] %d %d contenitore aggiornato: %d \n",id,tipo_processo,tipo_macchina,contenitore[tipo_macchina]);
				sem_post (&avvio_processo[tipo_macchina][P1]); //faccio ripartire
				pthread_mutex_unlock(&mut);
			} else {
				printf("processo [%d] %d %d errore sul tipo_processo\n",id,tipo_processo,tipo_macchina);
			}
		}
		else {
			//condizione di uscita verificata
			break;
		}
	}
	pthread_exit(NULL);
}

void* gestore(void* arg)
{
	int id = (int)arg;
	printf("gestore [%d] creato\n",id);
	while(1) {
		if (lap < nconfezioni)
		{
			sleep(10); //gestore super lento per verificare stop
			pthread_mutex_lock(&mut);
			if (contenitore[Pf]>0 && contenitore[Pl]>0 && contenitore[Pp]>0)
			{
				contenitore[Pf]--;
				contenitore[Pl]--;
				contenitore[Pp]--;
				lap++;
				printf("gestore [%d] nconfezioni creata\n",id);
				for (int i = 0; i < 3; ++i)
				{
					if (contenitore[i]==N-1) //era pieno prima di creazione ultima confezione
					{
						sem_post(&contenitore_non_pieno[i]);//sem_post dentro un mutex--> non bloccante
					}
				}
			}
			pthread_mutex_unlock(&mut);
			
		}
		else {
			pthread_mutex_unlock(&mut);
			//condizione di uscita verificata
			break;
		}
	}
	
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t processi[MAXT];
	pthread_t gest; //gestore del contenitore (PC)
	
	
	init();
	print_status();

	//creazione gestore
	if(pthread_create(&gest,NULL,gestore,(void *)0)<0)
	{
		fprintf(stderr, "errore creazione thread gestore\n");
		exit(1);
	}
	
	//creazione processi
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&processi[i],NULL,processo,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread processo [%d]\n",i);
			exit(1);
		}
	}
	
	//wait processi
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_join(processi[i],NULL)){
			fprintf(stderr, "errore terminazione thread processo [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread processo [%d] termiato con successo\n", i);
		}
	}

	//wait gestore
	if(pthread_join(gest,NULL)){
		fprintf(stderr, "errore terminazione thread gestore [%d]\n");
		exit(1);
	}
	else {
		printf("pthread gestore termiato con successo\n");
	}
	
	return 0;
}
