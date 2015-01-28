#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t my_sem;
int V=0;

void *thread1_process(void * arg)
{
	int sem_value;
	printf("[%d]-thread1 partito!\n",getpid());
	/*inizio fase 2*/
	sem_value = sem_wait (&my_sem);
	printf("[%d]-sem_value= [%d]\n",getpid(),sem_value);
	
	printf("[%d]-FASE2: thread1: V=%d\n",getpid(),V);
	pthread_exit(0);
}
void *thread2_process(void * arg)
{
	int sem_value;
	V=99;
	printf("[%d]-thread2 partito!\n",getpid());
	/*inizio fase 1*/
	printf("[%d]-FASE1: thread2: V=%d\n",getpid(),V);
	sem_value = sem_post (&my_sem);
	printf("[%d]-sem_value= [%d]\n",getpid(),sem_value);
	sleep(1);
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	pthread_t th1,th2;

	void *ret;//meaning?

	sem_init(&my_sem,0,0); /*semaforo a 0*/
	if(pthread_create(&th1,NULL,thread1_process,NULL)<0)
	{
		fprintf(stderr, "pthread_create error for thread 1\n");
		exit(1);
	}

	if(pthread_create(&th2,NULL,thread2_process,NULL)<0)
	{
		fprintf(stderr, "pthread_create error for thread 2\n");
		exit(1);
	}
	pthread_join(th1,&ret);
	pthread_join(th2,&ret);

	return 0;
}
