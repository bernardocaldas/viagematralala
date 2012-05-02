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
#include "protocol.h"


#define POOL_NO 1


sigset_t  set;

/* CUIDADO COM ESTA FUNCAO!!!*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void tratamento (int sigNumb){
	
}

void * servadmin (){
	char buffer[LEN],lixo[LEN],ctemp;
	bzero(buffer,LEN+1);
	printf("Insert command: \n");
	while(fgets(buffer,LEN,stdin)!=NULL){
	if(sscanf(buffer, "%c%s", &ctemp, lixo)==1){
		if(ctemp=='M'){
		}
		else if(ctemp=='F'){
		pthread_kill);
		}
		else{
		printf("Escreva comandos válidos\n");
		}
	} 
	else{
		printf("Escreva comandos válidos\n");
	}
	printf("Insert command: \n");
	}
}

void threadpool (){

	int i;
	pool_node * first_pool_node = create_pool();
	
	for(i=0; i< POOL_NO; i++){
		create_pool_node(&first_pool_node);		
	}
	
}


int main(int argc, char *argv[])
{
/* MUTEX */
	pthread_mutex_init(&mux, NULL);

/* FIFO */
	create_fifo(&front, &back);

/* SERVADMIN */
	pthread_t * servadmin_t;
	servadmin_t = (pthread_t *) malloc (1*sizeof(pthread_t));

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
     listen(sockfd,5); /* 5 means???*/
     clilen = sizeof(cli_addr);
     
	pthread_create(servadmin_t, NULL, servadmin, NULL);
	threadpool(); /* creates threadpool */
	 
	 sem_init(&sem_fifo, 0, 0);
	 
	 while(1){
     	
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          error("ERROR on accept");
        
        pthread_mutex_lock(&mux);
        /* Entrada na regiao critica*/
        queue (&front ,&back, newsockfd);
        sem_post(&sem_fifo);
        /* Saida da regiao critica*/
        pthread_mutex_unlock(&mux);
        
     }
     close(sockfd);
     return 0; 
}
