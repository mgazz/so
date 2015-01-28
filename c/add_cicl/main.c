/* tre processi che, ciclicamente, incrementano a
turno (in ordine P1,P2,P3) la variabile V*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX 5
static sem_t s[3];
int V;

void *add(void * arg)
{
	int id = (int)arg;
	int k=1;
	while(k) {
		sem_wait (&s[id]);
		if (V < MAX)
		{
			V++;
			printf("th [%d] - V=%d\n",id ,V);
		}
		else {
			//esco dal ciclo
			k=0;
			printf("th [%d] - esco dal ciclo\n",id);
		}
		//appuntati come ciclare su array
		printf("th [%d] - libero s [%d]\n",id ,(id+1)%3);
		sem_post (&s[(id+1)%3]);
	}
	printf("th [%d] - esco dal thread\n",id);
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	
	pthread_t th[3];
	//valore di partenza
	V=0;

	int i;

	sem_init(&s[0],0,1);
	sem_init(&s[1],0,0);
	sem_init(&s[2],0,0);

	//creo 3 thread
	for (i = 0; i < 3; ++i)
	{
		if(pthread_create(&th,NULL,add,(void *)i)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
	}
	
	for (i = 0; i < 3; ++i)
	{
		pthread_join(th[i],NULL);
		printf("th [%d] terminato\n", i);
	}
	
	return 0;
}
