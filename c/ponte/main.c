#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

// ho un ponte
// entrambe le direzioni -> corsia nord/sud a senso unico
// |vn-vs|<= K
// veicoli emergenza/ordinari
// favoriti in entrata veicoli emergenza

#define N 3
#define MAX 4
#define K 1

#define ND 1
#define SD 0

#define IN 1
#define OU 0

#define EM 1
#define OR 0

typedef struct mon
{
	int vn; //veicoli nord
	int vs; //veicoli sud
	pthread_mutex_t mut;
	//coda nord/sud-in/out-e/o
	pthread_cond_t coda[2][2][2];
	// auto in coda
	int vcoda[2][2][2];
};

//monitor
mon p;


// stampa status del monitor
void print_code(mon * m)
{
	int i,j,z;
	printf("------ stato code -----\n");
	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			for (z = 0; z < 2; ++z)
			{
				printf("coda[ %d ][ %d ][ %d ]: %d \n",i,j,z,m->vcoda[i][j][z]);
			}
		}
	}
	printf("------------------------\n");
}

void print_status(mon *m)
{
	printf("------ stato ponte-----\n");
	printf("vn: %d\t", m->vn);	
	printf("vs: %d\n", m->vs);	
	printf("|vn-ns| = %d\n",abs(m->vn - m->vs));
	printf("------------------------\n");
	
}

//inizializzazione del monitor
void init(mon* m)
{
	pthread_mutex_init(&m->mut,NULL);
	int i,j,z;
	char dir,vers,tipo;
	printf("----- inizializzazione variabili ------\n");
	pthread_mutex_init(&m->mut,NULL);
	m->vn = 0;
	m->vs = 0;
	printf("vn: %d\n", m->vn);	
	printf("vs: %d\n", m->vs);	
	printf("|vn-ns| = %d\n",abs(m->vn - m->vs));
	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			for (z = 0; z < 2; ++z)
			{
				pthread_cond_init(&m->coda[i][j][z],NULL);
				m->vcoda[i][j][z]=0;
				/*printf("coda[ %c ][ %c ][ %c ]: %d \n");dir,vers,tipo,m->vcoda[i][j][z];*/
				printf("coda[ %d ][ %d ][ %d ]: %d \n",i,j,z,m->vcoda[i][j][z]);
			}
		}
	}
	printf("------------------------\n");
}

void onord(mon* m,int id,int tipo)
{
	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==K && (m->vn < m->vs)
			) {
		printf("th[%d]-tipo[%d]coda out nord\n",id,tipo);

		m->vcoda[ND][OU][tipo]++;
		print_code(m);
		pthread_cond_wait(&m->coda[ND][OU][tipo],&m->mut);
		m->vcoda[ND][OU][tipo]--;
		print_code(m);
	}
	printf("th[%d]-tipo[%d] uscito dalla coda out nord\n",id,tipo);
	m->vn--;

	print_status(m);
	if (m->vcoda[ND][IN][EM]>0)
	{
		printf("th[%d]-start nord in em\n",id);
		pthread_cond_signal(&m->coda[ND][IN][EM]);
	}
	else if (m->vcoda[ND][IN][OR]>0) {
		printf("th[%d]-start sud in ord\n",id);
		pthread_cond_signal(&m->coda[ND][IN][OR]);
	}
	else if(m->vn > m->vs){ 
		if (m->vcoda[SD][IN][EM]>0)
		{
			printf("th[%d]-start sud out em\n",id);
			pthread_cond_signal(&m->coda[SD][OU][EM]);
		}
		else if (m->vcoda[SD][IN][OR]>0) {
			printf("th[%d]-start sud out ord\n",id);
			pthread_cond_signal(&m->coda[SD][OU][OR]);
		}
	}
	pthread_mutex_unlock(&m->mut);
}

void osud(mon* m,int id,int tipo)
{

	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==K && (m->vn > m->vs)
			) {
		printf("th[%d]-tipo[%d]coda out sud\n",id,tipo);
		m->vcoda[SD][OU][tipo]++;
		print_code(&m);
		pthread_cond_wait(&m->coda[SD][OU][tipo],&m->mut);
		m->vcoda[SD][OU][tipo]--;
		print_code(&m);
	}
	printf("th[%d]-tipo[%d] uscito dalla coda out sud\n",id,tipo);
	m->vs--;

	print_status(&m);

	if (m->vcoda[SD][IN][EM]>0)
	{
		printf("th[%d]-start sud in em\n",id);
		pthread_cond_signal(&m->coda[SD][IN][EM]);
	}
	else if (m->vcoda[SD][IN][OR]>0) {
		printf("th[%d]-start sud in ord\n",id);
		pthread_cond_signal(&m->coda[SD][IN][OR]);
	}
	else if(m->vn < m->vs){ 
		if (m->vcoda[ND][IN][EM]>0)
		{
			printf("th[%d]-start nord in em\n",id);
			pthread_cond_signal(&m->coda[ND][IN][EM]);
		}
		else if (m->vcoda[ND][IN][OR]>0) {
			printf("th[%d]-start nord in ord\n",id);
			pthread_cond_signal(&m->coda[ND][IN][OR]);
		}
	}
	pthread_mutex_unlock(&m->mut);
}


//in nord
void inord(mon* m,int id,int tipo)
{
	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)==K && (m->vn > m->vs) ||
			(tipo == OR && m->vcoda[ND][IN][EM]>0) ||
			(m->vn + m->vs) == MAX
				) {
		m->vcoda[ND][IN][tipo]++;
		print_code(m);
		pthread_cond_wait(&m->coda[ND][IN][tipo],&m->mut);
		m->vcoda[ND][IN][tipo]--;
		print_code(m);
	}
	printf("th[%d]-mezzo tipo [%d] entrato nord\n",id,tipo);

	m->vn++;
	
	print_status(m);

	//avendo terminato posso far partire un altro thread
	//scelgo il thread che mi fa rispettare i parametri
	if (m->vcoda[ND][OU][EM]>0)
	{
		printf("th[%d]-start nord out em\n",id);
		pthread_cond_signal(&m->coda[ND][OU][EM]);
	}
	else if (m->vcoda[SD][OU][OR]>0) {
		printf("th[%d]-start nord out ord\n",id);
		pthread_cond_signal(&m->coda[ND][OU][OR]);
	}
	else if (m->vcoda[SD][OU][EM]>0)
	{
		printf("th[%d]-start sud out em\n",id);
		pthread_cond_signal(&m->coda[SD][OU][EM]);
	}
	else if (m->vcoda[SD][OU][OR]>0) {
		printf("th[%d]-start sud out ord\n",id);
		pthread_cond_signal(&m->coda[SD][OU][OR]);
	}
	pthread_mutex_unlock(&m->mut);
}


//in sud
void isud(mon* m,int id,int tipo)
{
	pthread_mutex_lock(&m->mut);
	while(
			abs(m->vn - m->vs)>K ||
			abs(m->vn - m->vs)==K && (m->vn < m->vs) ||
			(tipo== OR && m->vcoda[SD][IN][EM]>0) ||
			(m->vn + m->vs) > MAX
				) {
		m->vcoda[SD][IN][tipo]++;
		print_code(&m);
		pthread_cond_wait(&m->coda[SD][IN][tipo],&m->mut);
		m->vcoda[SD][IN][tipo]--;
		print_code(&m);
	}
	printf("th[%d]-mezzo tipo [%d] entrato sud\n",id,tipo);

	m->vs++;
	print_status(&m);
	//avendo terminato posso far partire un altro thread
	//scelgo il thread che mi fa rispettare i parametri
	if (m->vcoda[SD][OU][EM]>0)
	{
		printf("th[%d]-start sud out em\n",id);
		
		pthread_cond_signal(&m->coda[SD][OU][EM]);
	}
	else if (m->vcoda[SD][OU][OR]>0) {
		printf("th[%d]-start sud out ord\n",id);
		pthread_cond_signal(&m->coda[SD][OU][OR]);
	}
	else if (m->vcoda[ND][OU][EM]>0)
	{
		printf("th[%d]-start nord out em\n",id);
		pthread_cond_signal(&m->coda[ND][OU][EM]);
	}
	else if (m->vcoda[ND][OU][OR]>0) {
		printf("th[%d]-start nord out ord\n",id);
		pthread_cond_signal(&m->coda[ND][OU][OR]);
	}
	pthread_mutex_unlock(&m->mut);
}

//process sud
void* psud(void* arg)
{
	int id = (int)arg;
	int tipo = rand()%2;
	printf("macchina [%d][%d][%d] parte\n",SD,IN,tipo);
	isud(&p,id,tipo);
	sleep(1);
	osud(&p,id,tipo);
	pthread_exit(NULL);
}

//process nord
void* pnord(void* arg)
{
	int id = (int)arg;
	int tipo = rand()%2;
	printf("macchina [%d][%d][%d] parte\n",ND,IN,tipo);
	inord(&p,id,tipo);
	sleep(1);
	onord(&p,id,tipo);
	pthread_exit(NULL);
	
}


int main(int argc, char *argv[])
{
	int i;
	int tipo;
	pthread_t th[N];
	init(&p);
	for (i = 0; i < N; ++i)
	{
		/*tipo=rand()%2;*/
		tipo = ND;
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
