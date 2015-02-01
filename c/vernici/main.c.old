#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define R 2
#define N 3
#define K 5

#define IN 1
#define OU 0

//monitor
typedef struct mon
{
	int u1;
	int u2;
	pthread_mutex_t mut;
	pthread_cond_t coda[2];
	int ncoda[2];
}

//operaio
typedef struct operaio
{
	int id;
	int tipo;
	int c_dim;
	int ver;
};

//recipiente
mon r;

// numero di spillate prima di fermarsi
int nspil;

//inizializzazione del monitor
void init(mon * m)
{
	int i;
	printf("----- inizializzazione monitor------\n");
	pthread_mutex_init(&m->mut,NULL);

	m->u1=0;
	m->u2=0;
	printf("u1=\n"m->u1);
	printf("u2=\n"m->u2);

	for (i = 0; i < 2; ++i)
	{
		//inizializzo le condizioni e le code
		pthread_cond_init(&m->coda[i]);
		m->coda[i]=0;
	}

	printf("------------------------\n");
}

//inserimento vernice
void ins(mon *m,operaio op)
{
	pthread_mutex_lock(&m->mut);
	//fase critica
	while(
			
			) {
		m->ncoda[IN]++;
		print_code(&m);;
		pthread_cond_wait(&m->ncoda[IN],&m->mut);
		m->ncoda[IN]--;
		print_code(&m);
	}

	pthread_mutex_unlock(&m->mut);
}

//spillatura vernice
void spill(mon *m,operaio op)
{
	pthread_mutex_lock(&m->mut);
	//fase critica
	while(
			
			) {
		m->ncoda[OU]++;
		print_code(&m);;
		pthread_cond_wait(&m->ncoda[OU],&m->mut);
		m->ncoda[OU]--;
		print_code(&m);
	}
	//sono in condizione di eseguire
	if (op.ver == U1)
	{
		m->u1-=op.ver;
		
	}

	pthread_mutex_unlock(&m->mut);
}

void start_ins(void *arg)
{
	operaio op= (operaio)arg;
	while(nspil<K) {
		ins(&r,op);
		sleep(1);
	}
}

void start_spill(void *arg)
{
	operaio op= (operaio)arg;
	while(nspil<K) {
		spill(&r,op);
		sleep(1);
	}
}

void start_spill(void *arg)
{
	op operaio = (operaio)arg;
	while(nspil<K) {
		spill(&r,op);
		sleep(1);
	}
}

operaio new_op(int tipo)
{
	operaio op;
	op.tipo = tipo;
	if(op.tipo==IN){
		// operaio inserisce 
		op.ver=rand()%2;
		op.c_dim=NULL;
	}
	else {
		//operaio spillatore
		//dimensione della latta
		op.ver=rand()%2;
		op.c_dim=rand()%N;
	}
	return op;
}

//sistema si ferma quanto effettuate K spillature
int main(int argc, char *argv[])
{
	int i;
	int tipo;
	int nthread=0;
	pthread_t th[2N];
	init(&p);
	operaio op;

	//creo inseritori
	for (i = 0; i < N; ++i)
	{
		op= new_op(IN);
		op.id=i;
		if (op.tipo)
		{
			if(pthread_create(&th[i],NULL,start_ins,(void *)op)<0)
			{
				fprintf(stderr, "pthread_create error for thread 1\n");
				exit(1);
			}
		}
	}

	printf("creati n inseritori\n");

	//creo spillatori
	for (i = N; i < 2N; ++i)
	{
		op= new_op(OU);
		op.id=i;
		//creazione di N spillatori
		if(pthread_create(&th[i],NULL,start_spill,(void *)op)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
		
	}

	printf("creati n inseritori\n");

	//aspetto terminazione 
	for (i = 0; i < 2N; ++i)
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
