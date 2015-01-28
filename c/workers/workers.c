#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREAD 5

void calcolo(void *t)
{
	int i;
	long tid,result=0;
	tid = (int)t;
	printf("thread %ld partito\n", tid);
	for (i = 0; i < 100; ++i)
	{
		result = result + tid;
	}
	printf("thread %ld finito: ris=%ld\n",tid,result);
	pthread_exit((void *) result);

}

int main(int argc, char *argv[])
{
	pthread_t thread[NUM_THREAD];
	int rc;
	int status;
	int i;
	for (i = 0; i < NUM_THREAD; ++i)
	{
		printf("creazione thread %ld\n", i);
		//pthread_create se corretto torna 1
		if(pthread_create(&thread[i],NULL,calcolo,(void *)i)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
		
	}
	for (i = 0; i < NUM_THREAD; ++i)
	{
		if(pthread_join(thread[i],(void *)&status)){
			fprintf(stderr, "pthread_join error for thread [%d] \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] termiato con successo\n", i);
		}
	}
	return 0;
}
