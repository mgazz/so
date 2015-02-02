/*
 * inseritoriI1: 
 *		ins una unit in U1
 *		privilegiati su I2
 * inseritoriI2: ins una unit in U2
 * inserimento ciclico
 * spillatori: 
 *		quando U1=U2*R
 *		spilla qta arbitraria di prodotto
 *		riempie una latta 
 *		privilegio latte + grandi
 * latte:
 *		formati: c1, c2,..,cn
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define MAXT 6
#define R 3
#define I1 0
#define I2 1
#define MAX 20
#define MAX1  15	// MAX1= MAX*R/(R+1)
#define MAX2 5		//MAX2=MAX/(R+1)
#define N 5			// numero formati latte: K=(R+1)*1, i=1,..N (N=MAX/(R+1) 

typedef struct
{
	int N1; // quantitÃ  di ingrediente I1  presente nel recipiente
	int N2; // quantitÃ  di ingrediente I2  presente nel recipiente
	int VersN1; // quantitÃ  di ingrediente I1 in fase di versamento
	int VersN2; // quantitÃ  di ingrediente I2 in fase di versamento
  int SpillI1; //quantitÃ  di ingrediente I1 in fase di spillatura
	int SpillI2; //quantitÃ  di ingrediente I2 in fase di spillatura
	
	pthread_cond_t codaI1; // coda portatori I1
	pthread_cond_t codaI2; // coda portatori I2
	pthread_cond_t codaS[N]; // code spillatori
	int sospI1; // portatori I1 sospesi 
	int sospI2; //portatori I2 sospesi
	int sospS[N]; //spillatori sospesi
	int caplatta[N]; // capacitÃ  latta
	pthread_mutex_t mutex;
} miscelatore;

miscelatore m;

void init(miscelatore* m)
{	
	int i;
	
	m->N1 = m->VemsN1=m->SpillI1=0;
	m->N2 = m->VemsN2=m->SpillI2=0;	
	m->sospI1 = m->sospI2= 0;
	pthread_cond_init(&m->codaI1, NULL);
	pthread_cond_init(&m->codaI2, NULL);
	for(i=0; i<N; i++)
	{	m->caplatta[i]=(1+R)*(i+1); // i=0 => (1+R); i=1 => 2*(1+R);...i=N-1 => N*(1+R)
		pthread_cond_init(&m->codaS[i], NULL);
		m->sospS[i]=0;
	}
	/*stampastato(m);*/
	pthread_mutex_init(&m->mutex, NULL);
}

void inizioS(miscelatore* r, int formato) // formato=0,.. N-1;
{	
	pthread_mutex_lock(&r->mutex);
	while (	versamentoON(r) ||	// c'Ã¨ qualcuno che sta versando
		   (formulaOK(r) && (r->caplatta[formato] > prodottoIN(r))) ||	// non c'Ã¨ prodotto sufficiente
		   !formulaOK(r) ||												// la formula non Ã¨ rispettata
		   (maggiore_priorita(&r, formato)))								// prioritÃ 
	{	r->sospS[formato]++;
	//	printf("Spillatore sospeso - latta %d di capacitÃ  %d --", formato, r->caplatta[formato]);
	//	stampastato(r);
		pthread_cond_wait(&r->codaS[formato], &r->mutex);
	//	printf("Spillatore risvegliato - latta %d di capacitÃ  %d\n", formato, r->caplatta[formato]);
	//	stampastato(r);
		r->sospS[formato]--;
	}
	r->SpillI1 += r->caplatta[formato]*R/(1+R);
	r->SpillI2 += r->caplatta[formato]/(1+R);
	//printf("spillatore IN: capacitÃ  %d ",  r->caplatta[formato]);
	//stampastato(r);
	pthread_mutex_unlock(&r->mutex);
}




void fineS(miscelatore* r, int formato)
{	pthread_mutex_lock(&r->mutex);
	
	r->SpillI1 -= r->caplatta[formato]*R/(1+R);
	r->SpillI2 -= r->caplatta[formato]/(1+R);
	r->N1 -= r->caplatta[formato]*R/(1+R);
	r->N2 -= r->caplatta[formato]/(1+R);
	if (!spillaturaON(r))
	{	if (r->sospI1)
		pthread_cond_broadcast(&r->coda1);
		if (r->sospI2)
			pthread_cond_broadcast(&r->coda2);	
	}
	printf("spillatore OUT: capacitÃ  %d ",  r->caplatta[formato]);
	stampastato(r);
	
	pthread_mutex_unlock(&r->mutex);
}

void inizioV(miscelatore* m, int ver)
{	
	pthread_mutex_lock(&m->mutex);

	if (ver ==I1) // I1
	{	
		while(	spillaturaON(m) ||	// c'Ã¨ qualcuno che sta spillando
			  m->N1+m->VersN1 == MAX1)  // il recipiente contiene il valore massimo per I1
		{	
			m->sospI1++;
			pthread_cond_wait(&m->coda1, &m->mutex);
			m->sospI1--;
		}
		m->VersN1++;
	}
	else if (ver==I2)
	{	
		while(	spillaturaON(m) ||				// c'Ã¨ qualcuno che sta spillando
			  (m->N2+m->VersN2 == MAX2)||		// il recipiente contiene il valore massimo per I2
			  ((m->sospI1>0) && (m->N1+m->VersN1<MAX1)))  // vincolo di prioritÃ  
	{	
		m->sospI2++;
		pthread_cond_wait(&m->coda2, &m->mutex);
		m->sospI2--;
	}
		m->VersN2++;
	}
	//printf("portatore IN, ingrediente %d\t", ver);
	//stampastato(m);
	pthread_mutex_unlock(&m->mutex);
}

void fineV(miscelatore* r, int ing)
{	
	int i;
	pthread_mutex_lock(&r->mutex);
	
	if (ing==I1)
	{	
		r->N1++;
		r->VersN1--;
	}
	else if (ing==I2)
	{	
		r->N2++;
		r->VersN2--;
	}
	if (	formulaOK(r) && // la formula Ã¨ rispettata 
			!versamentoON(r)) // non ci sono versamenti in corso
		for(i=N-1; i>=0; i--)
			pthread_cond_broadcast(&r->codaS[i]); // risveglio tutti gli spillatori in ordine di prioritÃ 
	else if ((ing==I1) && (r->sospI1==0)&&(r->sospI2))
		pthread_cond_broadcast(&r->coda2);
	
	printf("portatore OUT, ingrediente %d\n", ing);
	stampastato(r);
	pthread_mutex_unlock(&r->mutex);
}

int spillaturaON(miscelatore *r)
{	int ris;
	if ((r->SpillI1>0)||(r->SpillI2>0))
		ris=1;
	else ris=0;
	return ris;
}

void* portatore(void* arg)
{	int ing= (int)arg;
	while(1)
	{	inizioV(&m, ing);
		sleep(1);
		fineV(&m, ing);
	}
}

void* spillatore(void* arg)
{	
	int cap = (int)arg; //capacitÃ  latta
	inizioS(&m, cap);
	sleep(2);
	fineS(&m, cap);
	
}

int main()
{
	int i;
	pthread_t portatori[MAXT];
	pthread_t spillatori[MAXT];
	
	printf("Fabbrica vernici -- BENVENUTI!\n");
	
	srand(time(NULL));
	init(&m);
	
	sleep(2);
	
	for(i = 0; i < MAXT; i++)
	{
		pthread_create(&portatori[i], NULL, portatore, (void*)I1);
		pthread_create(&portatori[i], NULL, portatore, (void*)I2);
		pthread_create(&spillatori[i], NULL, spillatore, (void*)(rand() % N));
		pthread_create(&portatori[i], NULL, portatore, (void*)I1);
		
	}
	
	
	for(i = 0; i < MAXT; i++)
	{
		pthread_join(portatori[i], NULL);
		pthread_join(spillatori[i], NULL);
	}
	return 0;
}

