#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* vector;
int vl;
int m;

void *Calcolo(void *t) // codice worker
{ 
	int i;
	int tid, result=0;
	tid = (int)t;
	int num = vl/m;
	int lb,ub;
	int rest;
	lb=tid*num;
	ub=(tid+1)*num;
	rest = vl%m;
	if (rest!=0)
	{
		/*printf("th[%d] - rest: [%d]\n",tid,rest);*/
		if (tid<rest)
		{
			lb=lb+tid;
			ub=ub+tid+1;
		}
		else {
			lb+=rest;
			ub+=rest;
		}
	}
	/*printf("th[%d] è partito...\n",tid);*/
	/*printf("th[%d]: num=%d\n",tid, num);*/
	printf("th[%d]: range: [%d]-[%d]\n",tid, lb,ub);
	
	for (i=lb; i<ub; i++)
	{
		printf("td[%d] analize [%d]\n", tid,vector[i]);
		if (result<vector[i])
		{
			result=vector[i];
		}
	}
	/*printf("Thread %d ha finito. Ris= %d\n",tid, result);*/
	pthread_exit((void*) result);
}


int main (int argc, char *argv[])
{ 
	pthread_t *thread;
	int rc;
	long t;
	long status;
	int ok;
	int i;
	int nt;
	int test;
	int range;
	int result;
	printf("insert m\n");
	scanf("%d",&m);
	printf("m = %d\n", m);
	thread= (pthread_t*)malloc(m*sizeof(pthread_t));
	printf("insert vector lenght\n");
	scanf("%d",&vl);
	printf("insert range\n");
	scanf("%d",&range);
	printf("vl = %d\n", range);
	vector = (int *)malloc(vl*sizeof(int));
	for(i = 0; i < vl; ++i)
	{
		vector[i] = rand() % range + 1;
	}
	printf("vector:\n");
	for (i = 0; i < vl; ++i)
	{
		if (i%10==0 && i!=0)
		{
			printf("\n");
		}
		printf("[%d]\t",vector[i]);
	}
	printf("\n");
	
	for(t=0; t<m; t++) {
		printf("Main: creazione thread %ld\n", t);
		rc=pthread_create(&thread[t], NULL, Calcolo, (void *)t);
		if (rc) {
		 printf("ERRORE: %d\n", rc);
		 exit(-1); 
		}
	}
	for(t=0; t<m; t++) {
		rc = pthread_join(thread[t], (void *)&status);
		if (rc)
			printf("ERRORE join thread %ld codice %d\n", t, rc);
		else {
			printf("Finito thread %ld con ris. %ld\n",t,status);
			if (result<status)
			{
				result=status;
			}
		}
	}
	printf("highest value:%d \n",(int)result );
} 
