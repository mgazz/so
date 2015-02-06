/*in questo caso l'addetto che fa solo dei post non aspetta l'arrivo di tutti i passeggeri*/
#include "main.h"

//semafori
static sem_t biglietteria_libera;
static sem_t persona_esce_biglietteria;
static sem_t partenza_visita;


pthread_mutex_t mut;
pthread_cond_t tutti_dispositivi_riconsegnati;

int persone_uscite=0;
int persone_in_gruppo=0;
int ndipositivi=N;

void* persona(void* arg)
{
	int id = (int)arg;
	/*printf("persona %d creato\n",id);*/
	sem_wait (&biglietteria_libera);

	pthread_mutex_lock(&mut);
	persone_in_gruppo++;
	ndipositivi--;
	printf("persona %d esce dalla biglietteria ed entra nel gruppo\n",id);
	pthread_mutex_unlock(&mut);

	sem_post (&persona_esce_biglietteria);

	sem_wait(&partenza_visita);

	sleep(rand()%5+3);

	pthread_mutex_lock(&mut);
	ndipositivi++;
	persone_in_gruppo--;
	persone_uscite++;
	printf("persona %d restituisce il dispositivo: persone_uscite %d, ndipositivi %d\n",id,persone_uscite,ndipositivi);
	if (ndipositivi==N)
	{
		pthread_cond_signal(&tutti_dispositivi_riconsegnati);
		printf("persona %d ---risveglio biglietteria\n",id);
	}
	pthread_mutex_unlock(&mut);
	
	pthread_exit(NULL);
}

void* biglietteria(void* arg)
{
	int id = (int)arg;
	printf("biglietteria %d creato\n",id);
	while(1) {
		pthread_mutex_lock(&mut);
		printf("biglietteria %d persone uscite: %d\n",id,persone_uscite);
		if (persone_uscite < MAXT)
		{
			//do
			pthread_mutex_unlock(&mut);
			pthread_mutex_lock(&mut);
			if (persone_in_gruppo == N)
			{
				//partenza gruppo
				pthread_mutex_unlock(&mut);
				for (int i = 0; i < N; ++i)
				{
					sem_post (&partenza_visita);//faccio partire le persone nel gruppo
				}

				pthread_mutex_lock(&mut);
				while(ndipositivi!=N || persone_in_gruppo!=0) {
					printf("biglietteria %d aspetto tutti i dispositivi ritornati\n",id);
					pthread_cond_wait(&tutti_dispositivi_riconsegnati,&mut);
				}
				printf("biglietteria %d tutti disp riconsegnati \n",id);
				pthread_mutex_unlock(&mut);
			}
			else {
				printf("biglietteria %d aspetto prossima persona\n",id);
				pthread_mutex_unlock(&mut);
				sem_post (&biglietteria_libera);
				sem_wait (&persona_esce_biglietteria);
			}
		}
		else {
			//condizione verificata
			printf("biglietteria %d biglietteria termina\n",id);
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t persone[MAXT];
	pthread_t bigl;
	sem_init(&persona_esce_biglietteria,0,0);
	sem_init(&biglietteria_libera,0,0);
	sem_init(&partenza_visita,0,0);
	pthread_mutex_init(&mut,NULL);
	
	//creazione biglietteria
	if(pthread_create(&bigl,NULL,biglietteria,(void *)0)<0)
	{
		fprintf(stderr, "errore creazione thread biglietteria\n");
		exit(1);
	}
	
	//creazione persone
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_create(&persone[i],NULL,persona,(void *)i)<0)
		{
			fprintf(stderr, "errore creazione thread persona [%d]\n",i);
			exit(1);
		}
	}
	
	//wait persone
	for (int i = 0; i < MAXT; ++i)
	{
		if(pthread_join(persone[i],NULL)){
			fprintf(stderr, "errore terminazione thread persona [%d]\n",i);
			exit(1);
		}
		else {
			printf("thread persona [%d] termiato con successo\n", i);
		}
	}

	//wait biglietteria
	if(pthread_join(bigl,NULL)){
		fprintf(stderr, "errore terminazione thread biglietteria %d\n");
		exit(1);
	}
	else {
		printf("pthread biglietteria termiato con successo\n");
	}

	printf("----- fine------\n");
}
