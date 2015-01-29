#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
	int vcoda[2][1][2];
	int i,j,z;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (z = 0; z < 2; z++)
			{
				vcoda[i][j][z]=0;
				printf("coda[ %d ][ %d ][ %d ]: %d\n",i,j,z,vcoda[i][j][z]);
			}
		}
	}
	return 0;
}
