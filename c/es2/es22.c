
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define NUM_THREADS 3
#define K 2

typedef struct{
    int voti[K];
    char film[K][40];
    char vincitore[40];
    int pareri;
    pthread_mutex_t m;
} sondaggio;


typedef struct{
sem_t mb;
sem_t sb; 
int arrivati;
} barriera;

typedef struct{
pthread_mutex_t mf;
char vincitore[40];
} film_stream;

sondaggio S;
barriera B;
film_stream F; // film vincitore da trasmettere in streaming

void init(sondaggio *s, barriera *b) {
        int i;
        //inizializzazione sondaggio
        for(i=0;i<K; i++)
        {       printf("Qual Ã¨ il nome del film numero %d ?", i+1);
                gets(s->film[i]);
                s->voti[i]=0;
        }
        s->pareri=0;
        pthread_mutex_init(&s->m, NULL);
        // inizializzazione barriera:
        sem_init(&b->mb,0,1);
        sem_init(&b->sb,0,0);
        b->arrivati=0;
}

void esprimi_pareri(sondaggio *s, int th) {
    int i, voto;
    pthread_mutex_lock(&s->m);
    printf("\n\n COMPILAZIONE QUESTIONARIO per lo Spettatore %d:\n", th);
    for(i=0;i<K; i++)
	{	printf("voto del film  %s [0,.. 10]? ", s->film[i]);
		scanf("%d", &voto);
		s->voti[i]+=voto; //accumulo voti
	}
    s->pareri++;
    printf("FINE QUESTIONARIO per lo spettatore %d\n RISULTATI PARZIALI SONDAGGIO:\n", th);
    for(i=0;i<K;i++)
        printf("Valutazione media film %s: %f\n", s->film[i], (float)(s->voti[i])/s->pareri);
    pthread_mutex_unlock (&s->m);
}

film_stream winner(sondaggio *s)
{ int i_max, t;
  float max, media=0;
  film_stream result;
  
  printf("\n\n--- R I S U L T A T I ---\n");
  i_max=0; max=0;
  for(t=0; t<K;t++)
  {             media=(float) s->voti[t]/s->pareri;
                printf("Valutazione media del film n.%d (%s): %f\n", t+1, s->film[t], media);
                if (media>max)
                {       max=media;
                        i_max=t;
                }
  }
  printf("\n\n IL FILM VINCITORE E': %s, con voto %f !\n",  s->film[i_max], max);
  strcpy(s->vincitore, s->film[i_max]);
  // inizializzo film vincitore:
  strcpy(result.vincitore, s->film[i_max]);
  pthread_mutex_init(&result.mf, NULL); 

  return result;
}



void sync_barriera(barriera *b, sondaggio *s)
{       sem_wait(&b->mb);
        b->arrivati++; 
        if (b->arrivati==NUM_THREADS)
        {       F=winner(s);
                sem_post(&b->sb);
        }
        sem_post(&b->mb);
        sem_wait(&b->sb); 
        sem_post(&b->sb);
        return;
}

void *spettatore(void *t) // codice spettatore
{       long tid, result=0;
        tid = (int)t;
        esprimi_pareri(&S, tid);
        sync_barriera(&B, &S);
    printf("Spettatore %ld vede il film vincitore: %s\n",tid, F.vincitore); // visione film
        pthread_exit((void*) result);
}


int main (int argc, char *argv[])
{  pthread_t thread[NUM_THREADS];
   int rc;
   long t;
   
   void *status;
   
   init(&S, &B);
  
   for(t=0; t<NUM_THREADS; t++) {
      //printf("Main: creazione thread %ld\n", t);
      rc = pthread_create(&thread[t], NULL, spettatore, (void *)t); 
      if (rc) {
         printf("ERRORE: %d\n", rc);
         exit(-1);   }
  }
        for(t=0; t<NUM_THREADS; t++) {
      rc = pthread_join(thread[t], &status);
      if (rc) 
                   printf("ERRORE join thread %ld codice %d\n", t, rc);
   }
    
  return 0;
}

