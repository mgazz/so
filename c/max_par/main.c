#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define M 5

#define N 10

int *a;
int *b;


void calcolo(void *t)
{
	int i;
	int res = 0;
	int section = (int)t;
	printf("inizio thread [%d]\n",section);
	for (i = 0; i < N/M; ++i)
	{
		res = res + a[section*(N/M) + i] * b[section*(N/M) + i];
	}
	printf("fine thread [%d] - res: [%d]\n",i,res);
	pthread_exit(res);
}


int main(int argc, char *argv[])
{
	
	pthread_t th[M];
	int i;
	int final=0;
	int retval[M];

	a = (int*)malloc(N * sizeof(int));
	b = (int*)malloc(N * sizeof(int));

	for (i = 0; i < N; ++i)
	{
		printf(" [%d]\t",a[i] );
	}
	printf("\n" );

	for (i = 0; i < N; ++i)
	{
		printf(" [%d]\t",b[i] );
	}
	printf("\n" );

	for (i = 0; i < N; ++i)
	{
		printf(" [%d]\t",a[i] );
	}
	printf("\n" );

	for (i = 0; i < N; ++i)
	{
		printf(" [%d]\t",b[i] );
	}
	printf("\n" );


	//test
	for (i = 0; i < N; ++i)
	{
		final = final + a[i] * b[i];
	}
	printf("result: [%d]\n", final);

	final = 0;
	for (i = 0; i < M; ++i)
	{
		if(pthread_create(&th,NULL,calcolo,(void *)i)<0)
		{
			fprintf(stderr, "pthread_create error for thread 1\n");
			exit(1);
		}
		
	}
	for (i = 0; i < M; ++i)
	{
		if(pthread_join(th[i],(void *)&retval[i])){
			fprintf(stderr, "thread [%d] terminato con error \n",i);
			exit(1);
		}
		else {
			printf("pthread [%d] ritornato:[%d]\n",i,retval[i]);
		}
	}
	for (i = 0; i < M; ++i)
	{
		final = final + retval[i];
	}
	printf("valore finale: [%d]\n",final);
	
	return 0;
}
