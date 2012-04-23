/* 
Author: Allmighty Internet

Description: A simple server in the internet domain using STREAM
The port number is passed as an argument; 
unilateral communication;
each time a request is made a thread is created to deal with it;

Problems: makefile does not work with all the previous flags;
ctrl+D in client kills the server also
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include "yasc.h"
#include "fifo.h"
#include "pool.h"

#define POOL_NO 5


fifo_node * front;
fifo_node * back;

pthread_mutex_t mux;

sigset_t  set;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void tratamento (int sigNumb){
	
}

void * dispatcher (){
	fifo_node * current;
	pthread_t * threads;
	int i;
	int * sig;
	
	/* SIGNALS */
    	signal(SIGUSR1, tratamento);
	threads = (pthread_t *) malloc (POOL_NO*sizeof(pthread_t));

	/* INDEPENDENTE */ 
	/*
	for(i=0; i< POOL_NO; i++){
		if (pthread_create(&threads[i], NULL, yasc, NULL) != 0) 
		{
			error("ERROR creating thread\n");
			exit(-1);
		}
	}*/
	
	
	while (1){
		if(fifo_cnt == 0)
			pause();
			printf("Tratei o sinal\n");
		/* servidor acorda-o*/
		/* Entrada Regiao Critica */
		current = dequeue(&front,&back);
		pause();
		/* Saida Regiao Critica */
		if(free_cnt==0); /* free_cnt implica que cada thread actualize este contador; regiao critica no final do yasc */
			pause(); /* garantir que servidor não acorda este sinal; controlado pelas threads*/		
		/* Distribuição para a lista */	
		
		/* função que procura na lista a primeira thread livre. Vai encontrar uma thread livre (dada a condiçao anterior) e colocará no campo int * socket a informação relativa ao novo file descriptor. Em seguida envia um sinal para que a thread acorde */

	}
}


int main(int argc, char *argv[])
{
/* MUTEX */

	pthread_mutex_init(&mux, NULL);

/* FIFO */

	create_fifo(&front, &back);

/* THREADS */
	pthread_t * dispatcher_t;
	dispatcher_t = (pthread_t *) malloc (1*sizeof(pthread_t));

/* SOCKETS */
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     pid_t pid;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5); /* 5 significa o q?????*/
     clilen = sizeof(cli_addr);
     
	 if (pthread_create(dispatcher_t, NULL, dispatcher, NULL) != 0) 
	{
			error("ERROR creating dispatcher\n");
			exit(-1);
   	}
	 
	 while(1){
     	
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          error("ERROR on accept");
        
        pthread_mutex_lock(&mux);
        /* Entrada na regiao critica*/
        
        queue (&front ,&back, newsockfd);
        if(fifo_cnt == 1){ /* se fifo_cnt está a '1' então antes estava a zero */
        	/* Sinal para dispatcher */
        	/*pthread_kill(*dispatcher_t, SIGUSR1);*/
        }
        /* Saida da regiao critica*/
        pthread_mutex_unlock(&mux);
        
     }
     close(sockfd);
     return 0; 
}
