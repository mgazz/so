#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>

#define N 5

typedef struct 
{
	pthread_cond_t in_fila_per_il_contatore; 
	pthread_cond_t aspetta_il_via; 
	pthread_cond_t aspetta_contatore_uguale_a_cinque; 
	pthread_mutex_t mut;
	int contatore;
	int utenti_partiti
}monitor;

/*static sem_t visitatore_arrivato; //globale*/
/*static sem_t biglietteri_libera; //globale*/
/*static sem_t device_disponibili; //globale*/


monitor m; 

static sem_t condizione1;
static sem_t condizione2;
pthread_mutex_t sem_mutex;

int contatore;
int exe=1;
int utenti_partiti;

//ipotesi: dovrei avere problemi sul valore del contatore

void init()
{
	printf("-----init\n");
	pthread_mutex_init(&m.mut,NULL);
	pthread_cond_init(&m.in_fila_per_il_contatore,NULL);
	pthread_cond_init(&m.aspetta_il_via,NULL);
	pthread_cond_init(&m.aspetta_contatore_uguale_a_cinque,NULL);
	m.contatore=0;
	printf("m.contatore = %d\n",m.contatore);
	m.utenti_partiti=0;
	printf("m.utenti_partiti= %d\n",m.utenti_partiti);
}
/*void* user(void* arg)*/
/*{*/
	/*int id = (int)arg;*/
	/*printf("user [%d] start\n",id);*/
	/*printf("user [%d] aumenta contatore [%d]\n",id,++contatore);*/
	/*sem_wait (&condizione1);*/
	/*printf("[%d] passato\n",id);*/
	/*for (int i = 0; 1; ++i)*/
	/*{*/
		/*printf("[%d] faccio semwait\n",id);*/
		/*sem_wait (&condizione1);*/
		/*printf("passato %d volte\n",i+1);*/
	/*}*/
	/*pthread_exit(id);*/
	
/*}*/
/*void* barrier(void* arg)*/
/*{*/
	/*printf("barriera creata\n");*/
	/*while(exe) {*/
		/*printf("check_contatore\n");*/
		/*if (contatore==5)*/
		/*{*/
			/*printf("contatore==5\n");*/
			/*contatore=0;*/
			/*for (int i = 0; i < 5; ++i)*/
			/*{*/
				/*sem_post (&condizione1);*/
			/*}*/
			/*if (contatore==N)*/
			/*{*/
				/*exe=0;*/
			/*}*/
		/*}*/
	/*}*/
	/*pthread_exit(NULL);*/
/*}*/

void user_monitor(void* arg)
{
	int id = (int)arg;
	pthread_mutex_lock(&m.mut);
	printf("user [%d] start\n",id);
	printf("user [%d] preso il lock\n",id);
	while(m.contatore==5) {
		pthread_cond_wait(&m.in_fila_per_il_contatore,&m.mut);
	}
	m.contatore++;
	printf("user [%d] incrementato il contatore [%d]\n",m.contatore);
	if (m.contatore ==5)
	{
		printf("segnalo che il contatore uguale a 5\n");
		pthread_cond_signal(&m.aspetta_contatore_uguale_a_cinque)
	}
	printf("user [%d] aspetta il via dalla barriera\n");
	pthread_cond_wait(&m.aspetta_il_via,&m.mut);
	printf("user [%d] ricevuto via dalla barriera\n");

	pthread_mutex_lock(&m.mut);
	printf("user [%d] rilasciato il lock\n",id);
	
	pthread_exit(id);
}

void* barriera_monitor()
{
	printf("barriera creata\n");
	pthread_mutex_lock(&m.mut);
	printf("barriera ha preso il lock\n");
	while(exe) {
		while(
				m.contatore!=5
				) {
			printf("barriera aspetto contatore\n");
			pthread_cond_wait(&m.aspetta_contatore_uguale_a_cinque,&m.mut);
		}
			
		printf("contatore == 5 -> mando broadcast a utenti in fila\n");
		pthread_cond_broadcast(&m.aspetta_il_via);
		printf("barriera azzera il contatore\n");
		m.contatore=0;
	}
	
	printf("barriera rilascia il lock\n");
	pthread_mutex_unlock(&m.mut);
}


int main(int argc, char *argv[])
{
	pthread_t th[N];
	int retval[N];
	pthread_t bar;
	//inizializzazione mod monitor

	//inizializzazione mod semafori
	contatore=0;
	init();
	printf("creazione barriera_monitor\n");
	pthread_create(&bar,NULL,barriera_monitor, (void *)1);

	printf("creazione users\n");
	for (int i = 0; i < N; ++i)
	{
		pthread_create(&th[i],NULL,user_monitor, (void *)i);
	}

	for (int i = 0; i < N; ++i)
	{
		pthread_join(th[i],&retval[i]);
		printf("terminato son [%d]\n",retval[i]);
		
	}
	pthread_join(bar,NULL);

	//========== metodo errato
	/*sem_init(&condizione1,0,0);*/

	/*pthread_create(&bar,NULL,barrier, (void *)1);*/
	
	/*//creo il thread */
	/*printf("creazione users\n");*/
	/*for (int i = 0; i < N; ++i)*/
	/*{*/
		/*pthread_create(&th[N],NULL,user, (void *)i);*/
	/*}*/

	//aspetto i thread
	for (int i = 0; i < N; ++i)
	{
		pthread_join(th[i],&retval[i]);
		printf("terminato son [%d]\n",retval[i]);
		
	}
	pthread_join(bar,NULL);

	printf("main fine\n");
	
	return 0;
}
