#include "main.h"

#define MAXT 5 
#define MAX 5  //cap max forno
#define N 3 //cap max lotto

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
	int n_piastrelle[2];

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

void print_status()
{
	printf("\t\t-----	status	------\n");
	printf("\t\tm.temperatura: %d\n",m.temperatura);
	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.n_piastrelle[%d]:%d\n",i,m.n_piastrelle[i]);			
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				printf("\t\tm.coda[%d][%d][%d]: %d\n",i,j,k,m.coda[i][j][k]);
			}
		}
	}
	printf("\t\t------------------\n");
}

void init()
{
	pthread_mutex_init(&m.mut,NULL);
	for (int i = 0; i < 2; ++i)
	{
		m.n_piastrelle[i]=0;
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < N; ++k)
			{
				pthread_cond_init(&m.inserimento[i][j][k],NULL);
				m.coda[i][j][k]=0;
			}
		}
	}
	m.temperatura=0;
	print_status();
}


void signal()
{
	
}


//lotto entra
void in(lot l)
{
	pthread_mutex_lock(&m.mut);
	while((m.temperatura!=l.temperatura && m.n_piastrelle[m.temperatura]>0) || //temperatura diversa e forno occupato
				MAX - m.n_piastrelle[m.temperatura] < l.qta
				) {
		m.coda[l.temperatura][l.tipo][l.qta]++;
		print_status();
		pthread_cond_wait(&m.inserimento[T2][ST][l.qta],&m.mut);
		m.coda[l.temperatura][l.tipo][l.qta]--;
	}
	//entrata
	printf("lotto [%d] %d %d qta:%d entra \n",l.id,l.temperatura,l.tipo,l.qta);
	m.temperatura=l.temperatura;
	m.n_piastrelle[l.temperatura]+=l.qta;
	print_status();

	pthread_mutex_unlock(&m.mut);
}

//lotto esce
void out(lot l)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] %d %d esce\n",l.id,l.temperatura,l.tipo);
	m.n_piastrelle[l.temperatura]-=l.qta;
	print_status();
	/*signal();*/

	pthread_mutex_unlock(&m.mut);
}

void* lotto(void* arg)
{
	int id = (int)arg;
	printf("lotto [%d] creato\n",id);
	lot l; 
	l = new_lot(id);
	in(l);
	sleep(l.t);
	out(l);
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
