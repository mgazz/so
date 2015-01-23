#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIGUSR1 9
#define MAX 10

void sig_usr1_handler(int signum){/*Gestione del segnale*/
	printf("%d: received SIGUSR1(%d). Will terminate...\n", getpid(), signum);
}

void wait_for_signal(){
	/*Imposto il gestore dei segnali di tipo SIGUSR1*/
	signal(SIGUSR1,sig_usr1_handler);
	pause();
	exit(0);
}

void sleep_and_terminate(){
	sleep(5);
	printf("%d: Finished waiting 5 second\n", getpid());
	exit(0);
}
void wait_child() {
	int status,pid;
	pid = wait(&status);
	if ( WIFEXITED(status) ) {
		printf("Figlio %d terminato volontariamente con stato di terminazione %d\n",pid, WEXITSTATUS(status));
	}
	else if ( pid < 0)                                                                      
	{                                                                                       
		perror("Impossibile completare la wait");                                     
	}     
	else if ( WIFSIGNALED(status) )
	{
		printf("Figlio %d terminato dal segnale num. %d\n",pid, WTERMSIG(status));
	}
 /* Gestione condizioni di errore e verifica tipo di
terminazione (volontaria o da segnale) */
}

int main(int argc, char *argv[])
{
	int i,n_children,k,pid[MAX];

	// Print all argument (debug purpose)
	printf("Arguments:\n");
	for (i = 0; i < argc; i++)
		printf("[%d]:%s\n", i,argv[i]);

	//Check number of arguments
	if(argc < 2){
		printf("Illegal number of parameters\n");
		exit(1);
	}
	
	//Check argument is digit
	if(!isdigit(argv[1][0])){
		printf("%d parameter is not digit\n",argv[1][0]);
		exit(1);
	}
	n_children = atoi(argv[1]);
	k = (int)n_children/2;
	printf("n_children = %d\n", n_children);
	printf("k = %d\n", k);
	printf("----- start program -----\n");
	
	for (i = 0; i < n_children; ++i)
	{
		//printf("about to create child [%d]\n", i);
		pid[i] = fork();
		if (pid[i] == 0) // child
		{
			printf("child:pid - [%d][%d]\n",i,getpid());
			if (i<k){
				printf("\tchild[%d] about to start wait\n",getpid());
				wait_for_signal();
			}
			else {
				printf("\tchild[%d] wait 5 sec\n",getpid());
				sleep_and_terminate();
			}
		}
		else if (pid[i] > 0) {
			//father
		}
		else //Errors
		{
			perror("Impossibile fare la fork():");
			exit(2);
		}
	}
	for (i = 0; i < k; ++i)
	{
		kill(pid[i],SIGUSR1);
	}
	for (i = 0; i < n_children; ++i)
	{
		wait_child();
	}
	return 0;
}
