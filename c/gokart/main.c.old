#include "main.h"

#define MAXT 5 
#define MA 0
#define MI 1

#define C1 0
#define C2 1

#define N1 2
#define N2 2

typedef struct cl
{
	int id;
	int tipo;
}cl;

typedef struct monitor
{
	pthread_mutex_t mut;
	pthread_cond_t accesso_pista[2];
	int coda_accesso_pista[2];
	int kart_occupati[2];
	int kart_in_pista[2];

}monitor;

monitor m;

void init()
{
	pthread_mutex_init(&m.mut,NULL);//init
	for (int i = 0; i < 2; ++i)
	{
		m.kart_in_pista[i]=0;
		m.kart_occupati[i]=0;
		pthread_cond_init(&m.accesso_pista[i],NULL);
		m.coda_accesso_pista[i]=0;
	}
}

cl new_cl(int id)
{
	cl s; //struttura
	s.id = id;
	s.tipo= rand()%2;
	return s;
}

void print_status()
{
	printf("\t\t------status------\n");
	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.coda_accesso_pista[%d]: %d\n",i,m.coda_accesso_pista[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.kart_in_pista[%d]: %d\n",i,m.kart_in_pista[i]); 
	}
	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.kart_occupati[%d]: %d\n",i,m.kart_occupati[i]);
	}
	printf("\t\t------------------\n");
}

void signal()
{
	if (m.coda_accesso_pista[MA]>0)
	{
		pthread_cond_signal(&m.coda_accesso_pista[MA]);
	}
	else if (m.coda_accesso_pista[MI]>0) {
		pthread_cond_signal(&m.coda_accesso_pista[MI]);
	}
	else {
		printf("signal @ | code di accesso vuote!\n");
	}
}

//cliente  entra
void in(cl c)
{
	pthread_mutex_lock(&m.mut);
	if (c.tipo==MA)
	{
		while(
					m.kart_in_pista[MI]>0||
					m.kart_occupati[C1]==N1||
					m.kart_occupati[C2]==N2
					){ //condizione di wait
			m.coda_accesso_pista[MA]++;
			pthread_cond_wait(&m.accesso_pista[MA],&m.mut);
			m.coda_accesso_pista[MA]--;
		}
		if (m.kart_occupati[C1]<N1)
		{
			m.kart_occupati[C1]++;
		}
		else if (m.kart_occupati[C2]<N2) {
			m.kart_occupati[C2]++;
		}
		else {
			printf("cliente [%d]  errore su scelta cilindrata kart\n",c.id);
		}
		printf("cliente [%d]  consentito accesso\n",c.id);
		m.kart_in_pista[MA]++;
		print_status();
	}
	else if (c.tipo==MI) {
		while(
					m.kart_in_pista[MA]>0||
					m.kart_occupati[C2]==N2
					){ //condizione di wait
			m.coda_accesso_pista[MI]++;
			pthread_cond_wait(&m.accesso_pista[MI],&m.mut);
			m.coda_accesso_pista[MI]--;
		}
		if (m.kart_occupati[C2]<N2) {
			m.kart_occupati[C2]++;
		}
		else {
			printf("cliente [%d]  errore su scelta cilindrata kart\n",c.id);
		}
		printf("cliente [%d]  consentito accesso\n",c.id);
		m.kart_in_pista[MI]++;
		print_status();
	}
	pthread_mutex_unlock(&m.mut);
}

//cliente  esce
void out(cl c)
{
	pthread_mutex_lock(&m.mut);
	m.kart_occupati[c.tipo]--;
	m.kart_in_pista[c.tipo]--;
	printf("cliente [%d]  esce\n",c.id);
	print_status();
	signal();
	pthread_mutex_unlock(&m.mut);
}

void* cliente(void* arg)
{
	int id = (int)arg;
	cl c;
	c=new_cl(id);
	printf("cliente [%d] creato tipo: %d\n",id,c.tipo);

	in(c);
	sleep(rand()%5);
	out(c);
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	init();
	print_status();
	pthread_t clienti[MAXT];
	
	
	//creazione clienti
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&clienti[i],NULL,cliente,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread cliente [%d]\n",i);
			exit(1);
		}
	}
	
	//wait clienti
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_join(clienti[i],NULL)){
			fprintf(stderr, "errore terminazione thread cliente [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread cliente [%d] termiato con successo\n", i);
		}
	}

	
	return 0;
}
