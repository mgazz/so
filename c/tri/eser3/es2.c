#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for access() and exec
#include <sys/wait.h>
#include <signal.h>

void wait_child();
void signal_handler(int signo);
void child_code(char *command, int father_pid,int sec);
void wait_for_signal();
void signal_handler(int signum);

int pid[2];

int main(int argc, char *argv[])
{
	int i;
	int n_children;
	int father_pid;
	int sec;
	/*int pid[MAX];*/

	// Print all argument (debug purpose)
	printf("Arguments:\n");
	for (i = 0; i < argc; i++)
		printf("[%d]:%s\n", i,argv[i]);

	//Check number of arguments
	if(argc != 4){
		printf("Illegal number of parameters\n");
		exit(1);
	}
	
	//Check argument is digit
	if(!isdigit(argv[1][0])){
		printf("%d must be digit\n",argv[1][0]);
		exit(1);
	}

	//Check argument is positive
	if (argv[1][0]<=0)
	{
		printf("%d must be >0\n",argv[1][0]);
		exit(1);
	}
	//Check files exist and executable
	for (i = 2; i < argc-1; ++i)
	{
		if (access(argv[i],F_OK)!= -1 && access (argv[i],X_OK)!=-1)
		{
			printf("file: %s exist and executable\n", argv[i]);
		}
		else {
			printf("file: %s incorrect\n", argv[i]);
		}
	}
	

	sec = atoi(argv[1]);
	n_children = 2;
	printf("n_children = %d\n", n_children);
	signal(SIGUSR1,&signal_handler);
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
			child_code(argv[i+2],i,sec);
			
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

void child_code(char *command,int i,int sec){
	int result;
	printf("[%d]-execute command\n",getpid() );
	if (i==1)
	{
		printf("[%d]-I'm the second child - wait second\n",getpid());
		wait_for_signal(command ,sec);
	}
	else if (i==2) {
		printf("[%d]-I'm the first child - start command\n",getpid());
		/*result = execl(command,(char *)0);*/
		result =system(command);
		if (result == -1)
		{
			kill(pid[i+1%2],SIGTERM);
		}
		else {
			kill(pid[i+1%2],SIGUSR1);
		}		
	}


}

void wait_for_signal(char *command,int sec){
	int res;
	/*Imposto il gestore dei segnali di tipo SIGUSR1*/
	signal(SIGUSR1,&signal_handler);
	printf("[%d]-start sleep\n",getpid());
	res = sleep(sec);
	printf("[%d]-end sleep\n",getpid());
	//cosa succede se mentre dorme arriva un messaggio?
	//sleep ritorna 0 se non è stata interrotta
	if (res == 0)
	{
		system(command);
	}
}

void signal_handler(int signo)
{
    int source;
    switch(signo) {
        case SIGUSR1:
						printf("[%d]-process 2 terminate with success\n",getpid());
						printf("[%d]-executing command\n",getpid());
            break;
        /*case SIGUSR2:*/
            /*source = pid[1];*/
            /*break;*/
				case SIGTERM:
						printf("[%d]-process 2 terminate without success\n",getpid());
						exit(-1);
						break;
        default:
            fprintf(stderr, "Unespected signal\n");
            exit(5);
    }
    printf("Ricevuto un segnale da PID %d\n", source);
		printf("[%d]-received signal: %d \n",getpid(),signo);
		
    return;
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
}

