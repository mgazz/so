
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
	
	pthread_cond_t coda1; // coda portatori I1
	pthread_cond_t coda2; // coda portatori I2
	pthread_cond_t codaS[N]; // code spillatori
	int sospI1; // portatori I1 sospesi 
	int sospI2; //portatori I2 sospesi
	int sospS[N]; //spillatori sospesi
	int caplatta[N]; // capacitÃ  latta
	pthread_mutex_t mutex;
} miscelatore;

void stampastato(miscelatore* r);

miscelatore r;

void init(miscelatore* r)
{	int i;
	
	r->N1 = r->VersN1=r->SpillI1=0;
	r->N2 = r->VersN2=r->SpillI2=0;	
	r->sospI1 = r->sospI2= 0;
	pthread_cond_init(&r->coda1, NULL);
	pthread_cond_init(&r->coda2, NULL);
	for(i=0; i<N; i++)
	{	r->caplatta[i]=(1+R)*(i+1); // i=0 => (1+R); i=1 => 2*(1+R);...i=N-1 => N*(1+R)
		pthread_cond_init(&r->codaS[i], NULL);
		r->sospS[i]=0;
	}
	stampastato(r);
	pthread_mutex_init(&r->mutex, NULL);
}


void stampastato(miscelatore* r)
{	int i;
	if (formulaOK(r)&& !versamentoON(r))
		printf("\n --VERNICE PRONTA!! (quantitÃ  disponibile: %d)\n", prodottoIN(r));
	printf("\tN1:%d\t", r->N1); // quantitÃ  di ingrediente I1 effettivamente presente nel recipiente
	printf("N2:%d\t", r->N2); // quantitÃ  di ingrediente I2 effettivamente presente nel recipiente
	printf("VersN1:%d\t",r->VersN1); // quantitÃ  di ingrediente I1 in fase di versamento
	printf("VersN2:%d\t", r->VersN2); // quantitÃ  di ingrediente I2 in fase di versamento
    printf("SpillI1:%d\t", r->SpillI1); //quantitÃ  di ingrediente I1 in fase di spillatura
	printf("SpillI2:%d\t", r->SpillI2); //quantitÃ  di ingrediente I2 in fase di spillatura
	printf("SospI1:%d\t", r->sospI1); // portatori I1 sospesi 
	printf("SospI2:%d\t",  r->sospI2); //portatori I2 sospesi
	for(i=N-1; i>=0; i--)
		printf("\n\tsospS[%d]=%d\n", i, r->sospS[i]);
	
	
}


void inizioV(miscelatore* r, int ing)
{	pthread_mutex_lock(&r->mutex);
	if (ing ==0) // I1
	{	
		while(	spillaturaON(r) ||	// c'Ã¨ qualcuno che sta spillando
			  r->N1+r->VersN1 == MAX1)  // il recipiente contiene il valore massimo per I1
		{	
			r->sospI1++;
			pthread_cond_wait(&r->coda1, &r->mutex);
			r->sospI1--;
		}
		r->VersN1++;
	}
	else if (ing==I2)
	{	
		while(	spillaturaON(r) ||				// c'Ã¨ qualcuno che sta spillando
			  (r->N2+r->VersN2 == MAX2)||		// il recipiente contiene il valore massimo per I2
			  ((r->sospI1>0) && (r->N1+r->VersN1<MAX1)))  // vincolo di prioritÃ  
	{	
		r->sospI2++;
		pthread_cond_wait(&r->coda2, &r->mutex);
		r->sospI2--;
	}
		r->VersN2++;
	}
	//printf("portatore IN, ingrediente %d\t", ing);
	//stampastato(r);
	pthread_mutex_unlock(&r->mutex);
}

void fineV(miscelatore* r, int ing)
{	int i;
	pthread_mutex_lock(&r->mutex);
	
	if (ing==I1)
	{	r->N1++;
		r->VersN1--;
	}
	else if (ing==I2)
	{	r->N2++;
		r->VersN2--;
	}
	if (	formulaOK(r) && // la formula Ã¨ rispettata 
			!versamentoON(r)) // non ci sono versamenti in corso
		for(i=N-1; i>=0; i--)
			pthread_cond_broadcast(&r->codaS[i]); // risveglio tutti gli spillatori in ordine di prioritÃ 
	else if ((ing==I1) && (!r->sospI1)&&(r->sospI2))
		pthread_cond_broadcast(&r->coda2);
	
	printf("portatore OUT, ingrediente %d\n", ing);
	stampastato(r);
	pthread_mutex_unlock(&r->mutex);
}

void inizioS(miscelatore* r, int formato) // formato=0,.. N-1;
{	pthread_mutex_lock(&r->mutex);
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

int maggiore_priorita(miscelatore* r,int k) // verifica se ci sono spillatori sospesi con prioritÃ  piÃ¹ alta
{	int ris=0, i;
	if (k!=N-1)
		for (i=k+1; i<N; i++)
			if (r->sospS[i]>0)
				ris=1;
	return ris;
}

int prodottoIN(miscelatore *r)
{	int ris;
	ris= r->N1 - r->SpillI1 + r-> N2 - r->SpillI2;
	return ris;
}

int formulaOK(miscelatore *r)
{	int ris;
	if (((r->N1 - r->SpillI1)==(r->N2-r->SpillI2)*R) && (r->N1 - r->SpillI1>0) ) // la formula Ã¨ rispettata e il recipente non Ã¨ vuoto
		ris=1;
	else ris=0;
	return ris;
}

int versamentoON(miscelatore *r)
{	int ris;
	if ((r->VersN1>0)||(r->VersN2>0))
		ris=1;
	else ris=0;
	return ris;
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
	{	inizioV(&r, ing);
		sleep(1);
		fineV(&r, ing);
	}
}


void* spillatore(void* arg)
{	int k = (int)arg; //capacitÃ  latta
	inizioS(&r, k);
	sleep(2);
	fineS(&r, k);
	
}

int main()
{
	int i;
	pthread_t portatori[MAXT];
	pthread_t spillatori[MAXT];
	
	printf("Fabbrica vernici -- BENVENUTI!\n");
	printf("R vale %d, capacitÃ  massima miscela %d, capacitÃ  ingrediente 1: %d, capacitÃ  ingrediente 2: %d\n\n", R, MAX, MAX1, MAX2, N);
	
	srand(time(NULL));
	init(&r);
	
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

