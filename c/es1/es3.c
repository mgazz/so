#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
	
	
	//Check number of arguments
	if(argc <= 2){
		printf("Illegal number of parameters\n");
		exit(1);
	}
	
	//Check argument is digit
	if(!isdigit(argv[1][0])){
		printf("%d parameter is not digit\n",argv[1]);
		exit(1);
	}
	
	
	return 0;
}
