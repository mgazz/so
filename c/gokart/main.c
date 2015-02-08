#include "main.h"

#define MAXT 5 //clienti creati

#define N1 2
#define N2 2

#define MA 0
#define MI 1

#define C1 0
#define C2 1

typedef struct monitor
{
	pthread_mutex_t mut;
	pthread_cond_t entrata_cliente[2];
	int coda_entrata_cliente[2];
	int kart_in_pista[2];
	
	
	

}monitor;

monitor m;

typedef struct cl
{
	int id;
	int eta;
	int kart;
}cl;

cl new_cl(int id)
{
	cl s; //struttura
	s.id = id;
	s.eta= rand()%2;
	s.kart=-1;

	return s;
}

void print_cliente(cl s, char *message)
{
	printf("cliente [%d] | eta:%d kart:%d\t",s.id,s.eta,s.kart);
	print_message(message);
}

//cliente  entra
void in(cl* c)
{
	pthread_mutex_lock(&m.mut);
	//cliente entra in pista
	if (c->eta==MA) //adulto
	{
		while(
					(m.kart_in_pista[C1]==N1) || 
					(m.kart_in_pista[C2]==N2) 
					){ 
			print_cliente(*c,"entrato in wait");
			m.coda_entrata_cliente[c->eta]++;
			pthread_cond_wait(&m.entrata_cliente[c->eta],&m.mut);
			m.coda_entrata_cliente[c->eta]--;
		}
		if (m.kart_in_pista[C2]>0) //stanno girando C2
		{
			m.kart_in_pista[C2]++;
			c->kart=C2;
		}
		else { //stanno girando C1
			m.kart_in_pista[C1]++;
			c->kart=C1;
		}
	}
	else { //minorenne
			while(
						(m.kart_in_pista[C2]==N2) ||
						(m.kart_in_pista[C1]>0)
						){ 
				print_cliente(*c,"entrato in wait");
				m.coda_entrata_cliente[c->eta]++;
				pthread_cond_wait(&m.entrata_cliente[c->eta],&m.mut);
				m.coda_entrata_cliente[c->eta]--;
			}
			m.kart_in_pista[C2]++;
			c->kart=C2;
	}
	print_cliente(*c,"entra");
	print_status();
	pthread_mutex_unlock(&m.mut);
}

void signal()
{
	if (m.coda_entrata_cliente[MA]>0)
	{
		pthread_cond_signal(&m.entrata_cliente[MA]);
	} else if (m.coda_entrata_cliente[MI]>0){
		pthread_cond_signal(&m.entrata_cliente[MI]);
	} else {
		printf("@@@ code vuote @@@\n");
	}
}
//cliente  esce
void out(cl* c)
{
	pthread_mutex_lock(&m.mut);
	m.kart_in_pista[c->kart]--;
	print_cliente(*c,"esce");
	print_status();
	signal();
	pthread_mutex_unlock(&m.mut);
}

void* cliente(void* arg)
{
	int id = (int)arg;
	cl c;
	c=new_cl(id);
	/*print_cliente(c,"creato");*/
	sleep(rand()%5);
	in(&c);
	sleep(rand()%5+3);
	out(&c);
	
	pthread_exit(NULL);
}


void init()
{
	pthread_mutex_init(&m.mut,NULL);
	for (int i = 0; i < 2; ++i)
	{
		pthread_cond_init(&m.entrata_cliente[i],NULL);
		m.coda_entrata_cliente[i]=0;
		m.kart_in_pista[i]=0;
	}
}

void print_status()
{
	printf("\t\t------status------\n");

	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.coda_entrata_cliente[%d]: %d\n",i,m.coda_entrata_cliente[i]);
	}
	for (int i = 0; i < 2; ++i)
	{
		printf("\t\tm.kart_in_pista[%d]: %d\n",i,m.kart_in_pista[i]); 
	}

	printf("\t\t------------------\n");
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
