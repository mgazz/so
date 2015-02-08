#ifndef mainlib_h
#define mainlib_h value

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>
#include <time.h>

void print_message(char *message){
	printf("%s\n",message);
}

#endif
