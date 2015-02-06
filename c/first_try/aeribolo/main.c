/*in questo caso l'addetto che fa solo dei post non aspetta l'arrivo di tutti i passeggeri*/
#include "main.h"

static sem_t pass_prendi_kit; //globale
static sem_t pass_preso_kit; //globale
static sem_t check_doc_libero; //globale
static sem_t finito_check_dock; //globale
static sem_t accesso_aereo_libero; //globale
static sem_t pass_acceduto_aereo; //globale

pthread_mutex_t mut;
int kit_yes;
int kit_no;
int serviti;
void* t_passeggero(void* arg);
void* t_addetto(void* arg);


int main(int argc, char *argv[])
{
	int i;
	int retval[N];
	pthread_t passeggeri[N];
	pthread_t addetto;
	kit_yes=0;
	kit_no=0;
	serviti=0;

	

	//inizializzazione semaphore
	sem_init(&pass_prendi_kit,0,0);
	sem_init(&pass_preso_kit,0,0);
	sem_init(&check_doc_libero,0,0); 
	sem_init(&finito_check_dock,0,0); 
	sem_init(&accesso_aereo_libero,0,1);
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
	printf("=========== main fine ===========\n");
	
	return 0;
}

void* t_passeggero(void* arg)
{
	int id = (int)arg;
	/*printf("thread [%d] partito \n",id);*/
	printf("passeggero[%d]-\tattende per check_doc\n",id);
	sem_wait (&check_doc_libero);
	sleep(1);
	printf("passeggero[%d]-\tfinito_check_dock\n",id);
	sem_post (&finito_check_dock);
	if (rand()%2==PK) //il passeggero prende il kit
	{
		//scelto di prendere il kit
		//usa variabile per dire se prendi kit--> usa mutex
		pthread_mutex_lock(&mut);
		kit_yes++;
		printf("passeggero [%d]-\tkit_no=%d, kit_yes=%d\n",kit_no,kit_yes);
		/*printf("passeggero[%d]-\taumentato kit_yes: %d\n",id,kit_yes);*/
		pthread_mutex_unlock(&mut);
		printf("passeggero[%d]-\tscelto SI attende di prendere il kit\n",id);
		sem_wait (&pass_prendi_kit);
		sleep(rand()%2);
		pthread_mutex_lock(&mut);
		serviti++;
		pthread_mutex_unlock(&mut);
		printf("passeggero[%d]-\tpreso kit\n",id);
		sem_post (&pass_preso_kit);
	}
	else {
		//scelto di non prendere il kit e di continuare
		pthread_mutex_lock(&mut);
		kit_no++;
		serviti++;
		printf("passeggero [%d]-\tkit_no=%d, kit_yes=%d\n",kit_no,kit_yes);
		pthread_mutex_unlock(&mut);
		printf("passeggero[%d]-\tscelto NO kit va diretto per l'imbarco\n",id);
	}
	printf("passeggero [%d]-\tattende accesso aereo\n",id);
	sem_wait (&accesso_aereo_libero);
	sleep(rand()%5);
	printf("passeggero [%d]-\tacceduto aereo\n",id);
	sem_post (&accesso_aereo_libero);
	
	pthread_exit(id);
}
void* t_addetto(void* arg)
{
	int id = (int)arg;
	/*printf("thread [%d] partito \n",id);*/
	printf("addetto[%d]-partito apre il checkin\n",id);
	for (int i = 0; i < N; ++i)
	{
		sem_post (&check_doc_libero);
		printf("addetto[%d]-aspetto che check dock si liberi \n",id);
		sem_wait (&finito_check_dock);
	}

	printf("addetto[%d]-tutti i passeggeri hanno fatto check_in\n",id);
	printf("=========================================================\n");
	while(1) {
		pthread_mutex_lock(&mut);
		printf("addetto[%d]-serviti=%d\n",id,serviti);
		if (serviti<N)
		{
			printf("addetto[%d]-non tutti i passeggeri passati: kit_no=%d, kit_yes=%d\n",id,kit_no,kit_yes);
			if (kit_yes>0)
			{
				pthread_mutex_unlock(&mut);
				printf("addetto[%d]-faccio prendere kit al passeggero\n",id);
				sem_post (&pass_prendi_kit);
				sem_wait (&pass_preso_kit);
				printf("addetto[%d]-passeggero preso kit\n",id);
				
			}
			else {
				pthread_mutex_unlock(&mut);
			}
			
		}
		else {
			pthread_mutex_unlock(&mut);
			break;
		}
	}
	printf("addetto[%d]-tutti i passeggeri hanno passato la fase del kit\n",id);
	printf("=========================================================\n");
	/*for (int i = 0; i < N; ++i)*/
	/*{*/
		/*printf("addetto[%d]-faccio entrare in aereo il passeggero %d\n",id,i);*/
		/*sem_post (&accesso_aereo_libero);*/
		/*[>sem_wait (&pass_acceduto_aereo);<]*/
		/*printf("addetto[%d]-passeggero %d acceduto\n",id,i);*/
	/*}*/
	pthread_exit(NULL);
}

