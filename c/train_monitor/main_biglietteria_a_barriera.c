#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>

#define N 4
#define N_UT_IN_GR 2
#define N_GR_PR 2


typedef struct monitor
{
	pthread_cond_t biglietteria_libera; 
	pthread_cond_t partenza_gruppo; 
	pthread_cond_t finito_servire_utente; 
	pthread_cond_t gruppo_libero; 
	pthread_mutex_t mut;
	int n_in_gruppo;
	int n_in_coda_biglietteria;
	int n_gruppi_partiti;
	bool biglietteria_occupata;
}monitor;

/*static sem_t visitatore_arrivato; //globale*/
/*static sem_t biglietteri_libera; //globale*/
/*static sem_t device_disponibili; //globale*/


monitor m; 



//ipotesi: dovrei avere problemi sul valore del contatore

void init()
{
	printf("-----init\n");
	pthread_mutex_init(&m.mut,NULL);
	pthread_cond_init(&m.biglietteria_libera,NULL);
	pthread_cond_init(&m.partenza_gruppo,NULL);
	pthread_cond_init(&m.finito_servire_utente,NULL);
	pthread_cond_init(&m.gruppo_libero,NULL);
	m.n_in_gruppo=0;
	printf("m.n_in_gruppo= %d\n",m.n_in_gruppo);
	m.n_in_coda_biglietteria=0;
	printf("m.utenti_in_coda_biglietteria= %d\n",m.n_in_coda_biglietteria);
	m.biglietteria_occupata=true;
	m.n_gruppi_partiti=0;
	printf("m.n_gruppi_partiti= %d\n",m.n_gruppi_partiti);
}

void user(void* arg)
{
	int id = (int)arg;
	pthread_mutex_lock(&m.mut);
	/*printf("user [%d] start\n",id);*/
	/*printf("\tuser [%d] preso il lock\n",id);*/
	
	//stampa valori --> utente si rimette in fila!
	while( //m.n_in_gruppo==NG|| 
				/*m.n_in_coda_biglietteria>0 || //fila presente --> non passare davanti rude!*/
				m.biglietteria_occupata //biglietteria sta servendo un utente
				) {
		printf("\tuser [%d]aspetta in fila\n",id);
		m.n_in_coda_biglietteria++;
		pthread_cond_wait(&m.biglietteria_libera,&m.mut);
		printf("\tuser [%d]suo turno in biglietteria\n",id);
		m.n_in_coda_biglietteria--;
	}

	printf("\tuser [%d] arriva allo sportello della biglietteria\n",id);
	m.biglietteria_occupata=true;
	sleep(2);
	printf("\tuser [%d] finito procedura alla biglietteria\n",id);
	m.n_in_gruppo++;
	printf("\tuser [%d] passo la gestione alla biglietteria entro nel gruppo:%d\n",id,m.n_in_gruppo);

	pthread_cond_signal(&m.finito_servire_utente);
	printf("\tuser [%d] aspetto la partenza del gruppo\n",id);

	pthread_cond_wait(&m.partenza_gruppo,&m.mut);
	printf("\t\tuser [%d] partito con il gruppo\n",id);
	sleep(2);
	printf("\t\tuser [%d] terminato visita con il gruppo\n",id);
	m.n_in_gruppo--;
	pthread_cond_signal(&m.gruppo_libero);
	/*printf("\tuser [%d] rilasciato il lock\n",id);*/
	pthread_mutex_unlock(&m.mut);
	
	pthread_exit(id);
}

void* biglietteria()
{
	/*printf("barriera creata\n");*/
	pthread_mutex_lock(&m.mut);
	/*printf("\tbarriera ha preso il lock\n");*/

	while(m.n_gruppi_partiti!=N_GR_PR) {
		printf("----- gestione del gruppo %d -----\n",m.n_gruppi_partiti);
		printf("biglietteria apre\n");
		m.biglietteria_occupata=false;
		pthread_cond_signal(&m.biglietteria_libera);
		while(
				m.n_in_gruppo!=N_UT_IN_GR
				) {
		printf("biglietteria aspetta il prossimo utente, gruppo: %d\n",m.n_in_gruppo);
		m.biglietteria_occupata=false;
		pthread_cond_signal(&m.biglietteria_libera);
		pthread_cond_wait(&m.finito_servire_utente,&m.mut);
		}
		printf("biglietteria| gruppo pieno,chiudo biglietteria e faccio partire la visita\n");
		m.biglietteria_occupata=true;
			
		pthread_cond_broadcast(&m.partenza_gruppo);
		while(m.n_in_gruppo!=0) {
			printf("biglietteria| aspetta che il gruppo si svuoti, n_in_gruppo: %d\n",m.n_in_gruppo);
			pthread_cond_wait(&m.gruppo_libero,&m.mut);
		}
		m.n_gruppi_partiti++;
		m.biglietteria_occupata=false;
		pthread_cond_signal(&m.biglietteria_libera);
	}
	
	/*printf("barriera rilascia il lock\n");*/
	pthread_mutex_unlock(&m.mut);
	printf("biglietteria termina esecuzione\n");
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	pthread_t bigl;
	pthread_t th[N];
	int retval[N],ret_bigl;
	int i;
	//inizializzazione mod monitor

	//inizializzazione mod semafori
	/*cntatore=0;*/
	init();
	printf("creazione barriera_monitor\n");
	pthread_create(&bigl,NULL,biglietteria,NULL);

	printf("creazione users\n");
	for (int i = 0; i < N; ++i)
	{
		pthread_create(&th[i],NULL,user, (void *)i);
	}

	for (int i = 0; i < N; ++i)
	{
		pthread_join(th[i],&retval[i]);
		printf("terminato son [%d]\n",retval[i]);
		
	}
	printf("aspetto biglietteria \n");
	pthread_join(bigl,NULL);
	/*printf("terminata biglietteria %d\n",ret_bigl);*/
	printf("%d\n",i);

	/*printf("main fine\n");*/
	
	return 0;
}
