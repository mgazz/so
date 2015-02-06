#include "main.h"
typedef struct monitor
{
	pthread_mutex_t mut;
	int in_NF_S1;
	int in_NF_S2;
	int in_F_S2;
	int coda_NF;
	int coda_F;
	pthread_cond_t accesso_fumatori;
	pthread_cond_t accesso_non_fumatori;
	
}monitor;

monitor m;

void init()
{
	pthread_mutex_init(&m.mut,NULL);

	m.coda_F=0;
	printf("m.coda_F: %d\n",m.coda_F);
	m.coda_NF=0;
	printf("m.coda_NF: %d\n",m.coda_NF);
	
	m.in_F_S2=0;
	printf("m.in_F_S2: %d\n",m.in_F_S2);
	
	m.in_NF_S2=0;
	printf("m.in_NF_S2: %d\n",m.in_NF_S2);
	
	m.in_NF_S1=0;
	printf("m.in_NF_S1: %d\n",m.in_NF_S1);

	pthread_cond_init(&m.accesso_non_fumatori,NULL);
	pthread_cond_init(&m.accesso_fumatori,NULL);
	
}


//coppia entra
int inNF(int id)
{
	int sala_scelta;
	pthread_mutex_lock(&m.mut);
	while(
				(m.in_NF_S1==N1 && m.in_NF_S2==N2)|| //S1 e S2 occupate da NF
				//(m.in_NF_S1==N1 && m.in_F_S2==N2) || //S1 occupata da NF e S2 occupata da F --> ricado in caso successivo 
				(m.in_NF_S1==N1 && m.in_F_S2>0)	//sala 1 occupata e sala 2 contiene fumatori
				) {
		m.coda_NF++;//mi metto in coda
		pthread_cond_wait(&m.accesso_fumatori,&m.mut);
		m.coda_NF--;//esco dalla coda e riappaio
	}
	printf("coppia [%d] NF valuta sale: m.in_NF_S1=%d, m.in_NF_S2=%d\n",id,m.in_NF_S1,m.in_NF_S2);

	if (N1-m.in_NF_S1>0) //la sala 1 permette di entrare
	{
		m.in_NF_S1++;
		sala_scelta=S1;
		printf("coppia [%d] NF entra in S1\n",id);
	}
	else if(N2-m.in_NF_S2>0) {// per forza sala 2 non contiene fumatori e c'è spazio
		m.in_NF_S2++;
		sala_scelta=S2;
		printf("coppia [%d] NF entra in S2\n",id);
	}
	else {
		printf("*****coppia [%d] NF controllo entrata errato\n",id);
		
	}
	//controllo per ripartenza di occupa solo di priorita
	if (m.coda_NF>0)
	{
		pthread_cond_signal(&m.accesso_fumatori);
	}
	else if (m.coda_F>0) {
		pthread_cond_signal(&m.accesso_non_fumatori);
	}

	pthread_mutex_unlock(&m.mut);

	return sala_scelta;
}

//coppia esce
void outNF(int id, int sala_scelta)
{
	pthread_mutex_lock(&m.mut);
	//non ho vincoli di rimanenza nel sistema
	if (sala_scelta==S1)
	{
		m.in_NF_S1--;
		printf("coppia [%d] NF esce da S1\n",id);
	}
	else if(sala_scelta==S2){
		m.in_NF_S2--;
		printf("coppia [%d] NF esce da S2\n",id);
	}
	else {
		printf("######coppia [%d] NF sala_scelta controllo sbagliato\n",id);
		
	}

	if (m.coda_NF>0)
	{
		pthread_cond_signal(&m.accesso_fumatori);
	}
	else if (m.coda_F>0) {
		pthread_cond_signal(&m.accesso_non_fumatori);
	}
	pthread_mutex_unlock(&m.mut);
}

//coppia entra
void inF(int id)
{
	pthread_mutex_lock(&m.mut);
	while((m.in_NF_S2>0 || //presenti non fumatori in S2
				 m.in_NF_S2==N2)) {//sala piena in S2
		m.coda_F++;
		pthread_cond_wait(&m.accesso_non_fumatori,&m.mut);
		m.coda_F--;
	}
	printf("coppia [%d] F valuta sale: m.in_NF_S1=%d, m.in_NF_S2=%d\n",id,m.in_NF_S1,m.in_NF_S2);
	m.in_F_S2++;
	printf("coppia [%d] F entra in S2\n",id);

	if (m.coda_NF>0)
	{
		pthread_cond_signal(&m.accesso_fumatori);
	}
	else if (m.coda_F>0) {
		pthread_cond_signal(&m.accesso_non_fumatori);
	}


	pthread_mutex_unlock(&m.mut);
}

//coppia esce
void outF(int id)
{
	pthread_mutex_lock(&m.mut);
	m.in_F_S2--;

	printf("coppia [%d] F esce\n",id);
	if (m.coda_NF>0)
	{
		pthread_cond_signal(&m.accesso_fumatori);
	}
	else if (m.coda_F>0) {
		pthread_cond_signal(&m.accesso_non_fumatori);
	}

	pthread_mutex_unlock(&m.mut);
}

void* coppia(void* arg)
{
	int id = (int)arg;
	int sala_scelta;
	/*printf("coppia [%d] creato\n",id);*/
	int tipo;
	tipo= rand()%2;
	if (tipo==NF) //tipo non fumatore
	{
		printf("coppia [%d] tipo NF\n",id);
		sala_scelta=inNF(id);
		printf("coppia [%d] tipo NF entrata in sala %d\n",id,sala_scelta);
		sleep(rand()%5+5);
		outNF(id,sala_scelta);
	}
	else if (tipo==F) { //ultimo fumatore
		printf("coppia [%d] tipo F\n",id);
		inF(id);
		sleep(rand()%5);
		outF(id);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t coppie[MAXT];
	init();//in main
	
	
	//creazione coppie
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&coppie[i],NULL,coppia,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread coppia [%d]\n",i);
			exit(1);
		}
	}
	
	//wait coppie
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_join(coppie[i],NULL)){
			fprintf(stderr, "errore terminazione thread coppia [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread coppia [%d] termiato con successo\n", i);
		}
	}

	printf("----- fine------\n");
}
