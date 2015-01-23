#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
	int i,j,k,pid,status,n_children;
	char term;
	n_children = atoi(argv[1]);
	term = argv[2][0];
	for (i = 0; i < n_children; ++i)
	{
		pid = fork();
		if (pid == 0)
		{
			if (term == '1')
			{
				exit(0);
			}
		}
		else if (pid > 0) {
			printf("%d: child created with PID %d\n", getpid(),pid);
		}
		else {
			perror("Fork error:");
			exit(1);
		}
		
	}
	return 0;
}
