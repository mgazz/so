#include "main.h"

#define MAXT 5 
#define MAX 5  //cap max forno
#define N 2 //cap max lotto

#define T1 0
#define T2 1
#define SP 0 //speciale
#define ST 1 //standard

typedef struct monitor
{
	pthread_mutex_t mut;
	pthread_cond_t inserimento[2][2][N]; // T1/T2-ST/SP-card
	int coda[2][2][N];
	int temperatura;
	int n_piastrelle;
	
	
	

}monitor;

monitor m;

typedef struct lot
{
	int id;
	int t;
	int qta;
	int tipo;
	int temperatura;
}lot;

lot new_lot(int id)
{
	lot s; //struttura
	s.id = id;
	s.t= rand()%10; //tempo di cottura
	s.qta = rand()%N;
	s.temperatura = rand()%2; //T1/T2
	return s;
}

void init()
{
	pthread_mutex_init(&m.mut,NULL);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				pthread_cond_init(&m.inserimento[i][j][k],NULL);
				m.coda[i][j][k]=0;
				printf("m.coda[%d][%d][%d]: %d\n",i,j,k,m.coda[i][j][k]);
			}
		}
	}
	m.temperatura=0;
	printf("m.temperatura: %d\n",m.temperatura);
}


void signal()
{
	
}


//lotto T2 entra
void inT2(lot l)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] T2 entra \n",l.id);
	while((m.temperatura!=T2 && m.n_piastrelle>0) || //temperatura diversa e forno occupato
				MAX - m.n_piastrelle < l.qta
				) {
		if (l.tipo==ST)
		{
			m.coda[T2][ST][l.qta]++;
			pthread_cond_wait(&m.inserimento[T2][ST][l.qta],&m.mut);
			m.coda[T2][ST][l.qta]--;
		}
		else if (l.tipo==SP) {
			m.coda[T2][SP][l.qta]++;
			pthread_cond_wait(&m.inserimento[T2][SP][l.qta],&m.mut);
			m.coda[T2][SP][l.qta]--;
		}
		else {
			printf("lotto [%d] T2 errore sul tipo di piastrella\n",l.id);
		}
	}
	//entrata
	m.temperatura=T2;
	m.in_piastrelle+=l.qta;

	pthread_mutex_unlock(&m.mut);
}

//lotto T2 esce
void outT2(lot l)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] T2 esce\n",l.id);
	m.in_piastrelle-=l.qta;
	signal();

	pthread_mutex_unlock(&m.mut);
}
//lotto T1 entra
void inT1(lot l)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] T1 entra \n",l.id);
	while((m.temperatura!=T1 && m.n_piastrelle>0) || //temperatura diversa e forno occupato
				MAX - m.n_piastrelle < l.qta
				) {
		if (l.tipo==ST)
		{
			m.coda[T1][ST][l.qta]++;
			pthread_cond_wait(&m.inserimento[T2][ST][l.qta],&m.mut);
			m.coda[T1][ST][l.qta]--;
		}
		else if (l.tipo==SP) {
			m.coda[T1][SP][l.qta]++;
			pthread_cond_wait(&m.inserimento[T2][SP][l.qta],&m.mut);
			m.coda[T1][SP][l.qta]--;
		}
		else {
			printf("lotto [%d] T1 errore sul tipo di piastrella\n",l.id);
		}
	}
	//entrata
	m.temperatura=T1;
	m.in_piastrelle+=l.qta;

	pthread_mutex_unlock(&m.mut);
}

//lotto T1 esce
void outT1(lot l)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] T1 esce\n",l.id);
	m.in_piastrelle-=l.qta;
	signal();

	pthread_mutex_unlock(&m.mut);
}

void* lotto(void* arg)
{
	int id = (int)arg;
	printf("lotto [%d] creato\n",id);
	lot l; 
	l = new_lot(id);
	if (l.temperatura==T1)
	{
		inT1(l);
		sleep(l.t);
		outT1(l);
	}
	else if (l.temperatura==T2) {
		inT2(l);
		sleep(l.t);
		outT2(l);
		
	}
	else
		printf("lotto [%d] errore sul tipo\n",id);
	
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	init();//in main
	pthread_t lotti[MAXT];
	
	
	//creazione lotti
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&lotti[i],NULL,lotto,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread lotto [%d]\n",i);
			exit(1);
		}
	}
	
	//wait lotti
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_join(lotti[i],NULL)){
			fprintf(stderr, "errore terminazione thread lotto [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread lotto [%d] termiato con successo\n", i);
		}
	}
	
	return 0;
}
