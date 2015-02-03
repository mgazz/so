#include "main.h"

static sem_t check_doc; //globale
static sem_t prendi_kit; //globale
static sem_t accesso_aereo; //globale

pthread_mutex_t mut;
void* t_passeggero(void* arg)
{
	int id = (int)arg;
	/*printf("thread [%d] partito \n",id);*/
	printf("passeggero[%d]-va per check_doc\n",id);
	sem_wait (&check_doc);
	printf("passeggero[%d]-fatto check_doc, aspetta kit\n",id);
	if (rand()%2==PK) //il passeggero prende il kit
	{
		printf("passeggero[%d]-scelto SI kit\n",id);
		/*pthread_mutex_lock(&m);*/
		sem_wait (&prendi_kit);
		/*pthread_mutex_unlock(&m);*/
	}
	else {
		printf("passeggero[%d]-scelto NO kit\n",id);
	}
	printf("passeggero[%d]-aspetta di accesso aereo\n",id);
	printf("passeggero[%d]-salito su aereo\n",id);
	pthread_exit(id);
}
void* t_addetto(void* arg)
{
	int id = (int)arg;
	/*printf("thread [%d] partito \n",id);*/
	printf("addetto[%d]-partito\n",id);
	
	for (int i = 0; i < N; ++i)
	{
		printf("addetto[%d]-faccio passare il %d passeggero\n",id,i);
		sem_post (&check_doc);
	}

	printf("addetto[%d]-tutti i passeggeri passati\n",id);

	printf("addetto[%d]-faccio partire fila per kit\n",id);

	for (int i = 0; i < N; ++i)
	{
		printf("addetto[%d]-faccio prendere kit a passeggero\n",id);

		sem_post (&prendi_kit);
	}

	printf("addetto[%d]-apre porte imbarco\n",id);

	for (int i = 0; i < N; ++i)
	{
		printf("addetto[%d]-faccio salire passeggero\n",id);
		sem_post (&accesso_aereo);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i;
	int retval[N];
	pthread_t passeggeri[N];
	pthread_t addetto;
	

	//inizializzazione semaphore
	sem_init(&prendi_kit,0,0);
	sem_init(&check_doc,0,1); //parte disponibile
	sem_init(&accesso_aereo,0,0);
	pthread_mutex_init(&mut,NULL);

	//creazione addetto
	if(pthread_create(&addetto,NULL,t_addetto,(void *)i)<0)
	{
		fprintf(stderr, "thread [%d] creazione: error%d\n",i);
		exit(1);
	}
	
	//creazione passeggeri
	for (i = 0; i < N; ++i)
	{
		if(pthread_create(&passeggeri[i],NULL,t_passeggero,(void *)i)<0)
		{
			fprintf(stderr, "thread [%d] creazione: error%d\n",i);
			exit(1);
		}
	}
	
	//wait stop passeggeri
	for (i = 0; i < N; ++i)
	{
		if(pthread_join(passeggeri[i],(void *)&retval[i])){
			fprintf(stderr, "thread [%d] join: error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo, retval=[%d]\n", i,retval[i]);
		}
	}

	//wait stop addetto
	if(pthread_join(addetto,NULL)){
		fprintf(stderr, "thread [%d] join: error \n");
		exit(1);
	}
	else {
		printf("pthread termiato con successo,\n");
	}
	
	return 0;
}
