#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// ho un ponte
// entrambe le direzioni -> corsia nord/sud a senso unico
// |vn-vs|<= K
// veicoli emergenza/ordinari
// favoriti in entrata veicoli emergenza

#define N 5
#define max 4
#define k 4
#define n 1
#define s 0
#define e 1
#define o 0
#define in 0
#define out 0

typedef struct
{
	int vn; //veicoli nord
	int vs; //veicoli sud
	pthread_mutex_t mut;
	//coda nord/sud-in/out-e/o
	pthread_cond_t coda[2][2][2];
	// auto in coda
	int vcoda[2][2][2];
} mon;

//monitor
mon m;

void init(mon* m)
{
	pthread_mutex_init(&m->mut,NULL);
	int i,j,z;
	//inizializzazione 
	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			for (z = 0; z < 2; ++z)
			{
				pthread_cond_init(&m->coda[i][j][z],NULL);
				m->vcoda[i][j][z]=0;
			}
		}
	}
}

void onord(mon* m,int id,int tipo)
{

	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==k && (m->vn < m->vs)
			) {
		printf("th[%d]-tipo[%d]coda out nord\n",id,tipo);
		m->vcoda[n][out][tipo]++;
		pthread_cond_wait(&m->coda[n][out][tipo],&m->mut);
		m->vcoda[n][out][tipo]--;
	}
	printf("th[%d]-tipo[%d] uscito dalla coda out nord\n",id,tipo);
	m->vs--;

	if (m->vcoda[n][in][e]>0)
	{
		printf("th[%d]-start nord in em\n",id);
		pthread_cond_signal(&m->coda[n][in][e]);
	}
	else if (m->vcoda[n][in][o]>0) {
		printf("th[%d]-start sud in ord\n",id);
		pthread_cond_signal(&m->coda[n][in][o]);
	}
	else if(m->vn > m->vs){ 
		if (m->vcoda[s][in][e]>0)
		{
			printf("th[%d]-start sud out em\n",id);
			pthread_cond_signal(&m->coda[s][out][e]);
		}
		else if (m->vcoda[s][in][o]>0) {
			printf("th[%d]-start sud out ord\n",id);
			pthread_cond_signal(&m->coda[s][out][o]);
		}
	}
	pthread_mutex_unlock(&m->mut);
}

void osud(mon* m,int id,int tipo)
{

	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==k && (m->vn > m->vs)
			) {
		printf("th[%d]-tipo[%d]coda out sud\n",id,tipo);
		m->vcoda[s][out][tipo]++;
		pthread_cond_wait(&m->coda[s][out][tipo],&m->mut);
		m->vcoda[s][out][tipo]--;
	}
	printf("th[%d]-tipo[%d] uscito dalla coda out sud\n",id,tipo);
	m->vs--;

	if (m->vcoda[s][in][e]>0)
	{
		printf("th[%d]-start sud in em\n",id);
		pthread_cond_signal(&m->coda[s][in][e]);
	}
	else if (m->vcoda[s][in][o]>0) {
		printf("th[%d]-start sud in ord\n",id);
		pthread_cond_signal(&m->coda[s][in][o]);
	}
	else if(m->vn < m->vs){ 
		if (m->vcoda[n][in][e]>0)
		{
			printf("th[%d]-start nord in em\n",id);
			pthread_cond_signal(&m->coda[n][in][e]);
		}
		else if (m->vcoda[n][in][o]>0) {
			printf("th[%d]-start nord in ord\n",id);
			pthread_cond_signal(&m->coda[n][in][o]);
		}
	}
	pthread_mutex_unlock(&m->mut);
}


//in nord
void inord(mon* m,int id,int tipo)
{
	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==k && (m->vn > m->vs) ||
			(tipo==o && m->vcoda[n][in][e]>0) ||
			(m->vn + m->vs) == max
				) {
		m->vcoda[n][in][tipo]++;
		pthread_cond_wait(&m->coda[n][in][tipo],&m->mut);
		m->vcoda[n][in][tipo]--;
	}
	printf("th[%d]-mezzo tipo [%d] entrato nord\n",id,tipo);
	/*print_status();*/
	m->vn++;

	//avendo terminato posso far partire un altro thread
	//scelgo il thread che mi fa rispettare i parametri
	if (m->vcoda[n][out][e]>0)
	{
		printf("th[%d]-start nord out em\n",id);
		pthread_cond_signal(&m->coda[n][out][e]);
	}
	else if (m->vcoda[s][out][o]>0) {
		printf("th[%d]-start nord out ord\n",id);
		pthread_cond_signal(&m->coda[n][out][o]);
	}
	else if (m->vcoda[s][out][e]>0)
	{
		printf("th[%d]-start sud out em\n",id);
		pthread_cond_signal(&m->coda[s][out][e]);
	}
	else if (m->vcoda[s][out][o]>0) {
		printf("th[%d]-start sud out ord\n",id);
		pthread_cond_signal(&m->coda[s][out][o]);
	}
	pthread_mutex_unlock(&m->mut);
}


//in sud
void isud(mon* m,int id,int tipo)
{
	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)>k ||
			abs(m->vn - m->vs)==k && (m->vn < m->vs) ||
			(tipo==o && m->vcoda[s][in][e]>0) ||
			(m->vn + m->vs) > max
				) {
		m->vcoda[s][in][tipo]++;
		pthread_cond_wait(&m->coda[s][in][tipo],&m->mut);
		m->vcoda[s][in][tipo]--;
	}
	printf("th[%d]-mezzo tipo [%d] entrato sud\n",id,tipo);
	m->vs++;

	//avendo terminato posso far partire un altro thread
	//scelgo il thread che mi fa rispettare i parametri
	if (m->vcoda[s][out][e]>0)
	{
		printf("th[%d]-start sud out em\n",id);
		
		pthread_cond_signal(&m->coda[s][out][e]);
	}
	else if (m->vcoda[s][out][o]>0) {
		printf("th[%d]-start sud out ord\n",id);
		pthread_cond_signal(&m->coda[s][out][o]);
	}
	else if (m->vcoda[n][out][e]>0)
	{
		printf("th[%d]-start nord out em\n",id);
		pthread_cond_signal(&m->coda[n][out][e]);
	}
	else if (m->vcoda[n][out][o]>0) {
		printf("th[%d]-start nord out ord\n",id);
		pthread_cond_signal(&m->coda[n][out][o]);
	}
	pthread_mutex_unlock(&m->mut);
}

//process sud
void* psud(void* arg)
{
	int id = (int)arg;
	int tipo = rand()%2;
	isud(&m,id,tipo);
	sleep(1);
	osud(&m,id,tipo);
	pthread_exit(NULL);
}

//process nord
void* pnord(void* arg)
{
	int id = (int)arg;
	int tipo = rand()%2;
	inord(&m,id,tipo);
	sleep(1);
	onord(&m,id,tipo);
	pthread_exit(NULL);
	
}


int main(int argc, char *argv[])
{
	int i;
	int tipo;
	pthread_t th[N];
	for (i = 0; i < N; ++i)
	{
		tipo=rand()%2;
		if (tipo)
		{
			if(pthread_create(&th[i],NULL,pnord,(void *)i)<0)
			{
				fprintf(stderr, "pthread_create error for thread 1\n");
				exit(1);
			}
		}
		else {
			if(pthread_create(&th[i],NULL,psud,(void *)i)<0)
			{
				fprintf(stderr, "pthread_create error for thread 1\n");
				exit(1);
			}
		}
	}
	//aspetto risultati
	for (i = 0; i < N; ++i)
	{
		if(pthread_join(th[i],NULL)){
			fprintf(stderr, "thread [%d] terminato con error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo\n", i);
		}
	}
	
	
	return 0;
}
