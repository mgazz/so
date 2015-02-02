typedef struct monitor
{
	int utente;
	pthread_cond_t coda[N]; 
	int ncoda[N]; 
	pthread_mutex_t mut;
} ;

monitor m; 

void init(monitor* m)
{
	int i;
	printf("----- inizializzazione monitor ------\n");
	m->utente=0;
	printf("m->utente: %d\n", m->utente);	
	

	for (i = 0; i < N; ++i)
	{
		pthread_cond_init(&m->coda[i],NULL);
		m->ncoda[i]=0;
		printf("ncoda[ %d ][ %d ][ %d ]: %d \n",i,m->ncoda[i]);
	}
	pthread_mutex_init(&m->mut,NULL);
	printf("---------------------------------------\n");
}
int main(int argc, char *argv[])
{
	
	return 0;
}
