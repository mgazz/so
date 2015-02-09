#include "main.h"

#define MAXT 5 

typedef struct monitor
{
	pthread_mutex_t mut;
	pthread_cond_t entrata[3][4][9];
	int coda_entrata[3][4][9];
	
	//init

}monitor;

monitor m;

void init()
{
	pthread_mutex_init(&m.mut,NULL);
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 9; ++k)
			{
				pthread_cond_init(&m.entrata[i][j][k],NULL);
				m.coda_entrata[i][j][k]=0;
			}
		}
	}
}

void print_status()
{
	printf("\t\t------status------\n");
	for (int i = 0; i < 3; ++i)
	{
			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 9; ++k)
				{
					printf("\t\tm.coda_entrata[%d][%d][%d]: %d\n",i,j,k,m.coda_entrata[i][j][k]);
				}
			}
	}
	
	printf("\t\t------------------\n");
}

int main(int argc, char *argv[])
{
	init();
	print_status();
	
	return 0;
}
