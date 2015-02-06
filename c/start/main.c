#include "main.h"

#define MAXT 5 
#define N 5  
#define MAX

#define T1 0
#define T2 1
#define ST 0
#define SP 1

typedef struct monitor
{
	pthread_mutex_t mut;
	int stato_temperatura;
	int in_npiastelle; //piastrelle all'interno del forno
	pthread_cond_t avvia_lotto_t1;
	pthread_cond_t avvia_lotto_t2;
	pthread_cond_init(&m.avvia_lotto_t2,NULL);
	pthread_cond_init(&m.avvia_lotto_t1,NULL);
	
	

}monitor;

monitor m;

void init()
{
	pthread_mutex_init(&m.mut,NULL);
	m.stato_temperatura=T2; //parto con piastrelle privilegiate
	printf("m.stato_temperatura: %d\n",m.stato_temperatura);
	m.in_npiastelle=0;
	printf("m.in_npiastelle: %d\n",m.in_npiastelle);
	
}

//lotto t2 entra
void int2(int id)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] t2 entra \n",id);

	pthread_mutex_unlock(&m.mut);
}

//lotto t2 esce
void outt2(int id)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] t2 esce\n",id);

	pthread_mutex_unlock(&m.mut);
}

//lotto t1 entra
void int1(int id)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] t1 entra \n",id);

	pthread_mutex_unlock(&m.mut);
}

//lotto t1 esce
void outt1(int id)
{
	pthread_mutex_lock(&m.mut);
	printf("lotto [%d] t1 esce\n",id);

	pthread_mutex_unlock(&m.mut);
}

void* lotto(void* arg)
{
	int id = (int)arg;
	int temperatura;
	int t;
	int tipo_lotto;
	int n_piastrelle;

	printf("lotto [%d] creato\n",id);
	temperatura= rand()%2; //T1 or T2
	tipo_lotto= rand()%2; //tipo standard-speciale
	n_piastrelle=rand()%N;
	t = rand()%10;//tempo di rimanenza del lotto all'interno del forno

	if (temperatura==T1)
	{
		//lotto di tipo t1
		int1(id);
		sleep(rand()%5);
		outt1(id);
	}
	else if(temperatura==T2){
		//lotto di tipo t2
		int2(id);
		sleep(rand()%5);
		outt2(id);
	}
	else {
		printf("lotto [%d] errore sul tipo\n",id);
		
	}
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	
	pthread_t lotti[MAXT];
	
	init();//in main
	
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

	printf("----- fine------\n");
}
