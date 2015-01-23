#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void wait_child();
void signal_handler(int signo);
void son_code(int num_son, int father_pid);

int pid[3];

int main(int argc, char *argv[])
{
	int i;
	int n_children;
	int father_pid;
	int pid[MAX];

	// Print all argument (debug purpose)
	printf("Arguments:\n");
	for (i = 0; i < argc; i++)
		printf("[%d]:%s\n", i,argv[i]);

	//Check number of arguments
	if(argc < 4){
		printf("Illegal number of parameters\n");
		exit(1);
	}
	
	//Check argument is digit
	if(!isdigit(argv[1][0])){
		printf("%s must be digit\n",argv[1][0]);
		exit(1);
	}
	if (argv[1][0]<=0)
	{
		printf("%s must be >0\n",argv[1][0]);
		exit(1);
	}
	sec = atoi(argv[1]);
	n_children = 3;
	printf("n_children = %d\n", n_children);
	signal(SIGUSR1,&signal_handler);
	signal(SIGUSR2,&signal_handler);
	signal(SIGTERM,&signal_handler);

	printf("----- start program -----\n");
	//Pid of the father
	father_pid = getpid();
	
	for (i = 0; i < n_children; ++i)
	{
		//printf("about to create child [%d]\n", i);
		pid[i] = fork();
		if (pid[i] == 0)	//child
		{
			printf("child:pid - [%d][%d]\n",i,getpid());
			
		}
		else if (pid[i] > 0)//father
		{}
		else //Errors
		{
			perror("Unable to fork()");
			exit(2);
		}
	}

	//father code
	for (i = 0; i < n_children; ++i)
	{
		wait_child();
	}
	return 0;
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

void son_code(int num, int father_pid)
{
    int rand_num, delay, signo;
    /* 
     * Generazione del numero casuale.
     * Ognuno dei figli inizializza la sequenza di numeri casuali
     * con il suo pid() per garantire che le sequenze generate dalla
     * rand() siano diverse
     */ 
    srand(getpid());
    rand_num = rand();
    delay = rand_num % 6;
    sleep(delay);

    /*Se delay è pari*/
    if ( (delay % 2) == 0) 
    {
        int kill_success;
        switch (num) {
            case 0: /*Primo figlio*/
                signo = SIGUSR1;
                break;
            case 1: /*Secondo figlio*/
                signo = SIGUSR2;
                break;
            case 2: /*Terzo figlio*/
                signo = SIGTERM;
                break;
            default:
                fprintf(stderr, "Numero di figlio inaspettato!\n");
                exit(3);
        }

        printf("%d: invio segnale\n", getpid());
        kill_success = kill( parent_pid, signo); 
        if (kill_success < 0) 
        {
            perror("Impossibile inviare il segnale:");
            exit(6);
        }
    }

    exit(0);
}

void signal_handler(int signo)
{
    int source;
    switch(signo) {
        case SIGUSR1:
            source = pid[0];
            break;
        case SIGUSR2:
            source = pid[1];
            break;
        case SIGTERM:
            source = pid[2];
            break;
        default:
            fprintf(stderr, "Segnale inaspettato\n");
            exit(5);
    }
    printf("Ricevuto un segnale da PID %d\n", source);
    return;
}
