/* 
Authors: 

67557 Bernardo Caldas
67636 João Silva

Description: A simple server in the internet domain using STREAM
The port number is passed as an argument; 
unilateral communication;
each time a request is made a thread is created to deal with it;

Problems: makefile does not work with all the previous flags;
the accept command creates a new socket even when the maximum FIFO size is met
solution: accept all sockets the way we are doing but write TRUE or FALSE whether
the FIFO is full or not so that the client can try connecting again or not;
*/
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "yasc.h"
#include "fifo.h"
#include "pool.h"
#include "protocol.h"


sigset_t  set;

void display_client_info(){
	pool_node *aux;
	aux=first_pool_node;
	struct sockaddr_in sockinfo;
	int addrlen;
	addrlen=sizeof(sockinfo);
	printf("Info de clientes:\n");
	while(aux!= NULL){
		if(aux->socket!=0)
		{
			getpeername(aux->socket,(struct sockaddr *)&sockinfo,&addrlen);
			printf("Cliente com endereço IP %s\n",inet_ntoa(sockinfo.sin_addr));
		}
		aux=aux->next;
	}


}

void tratamento (int sigNumb){
	printf("Signal received\n");
}

void * servadmin (){
	char buffer[LEN],lixo[LEN],ctemp;
	bzero(buffer,LEN+1);
	printf("Insert command: \n");
	while(fgets(buffer,LEN,stdin)!=NULL){
	if(sscanf(buffer, "%c%s", &ctemp, lixo)==1){
		if(ctemp=='M'){
			display_client_info();
		}
		else if(ctemp=='F'){
		raise(SIGUSR1);
		/*TEMP*/ exit(1);
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
	pool_no = POOL_INIT;
	first_pool_node = create_pool();
	
	for(i=0; i< pool_no; i++){
		create_pool_node(&first_pool_node);		
	}
	
}


int main(int argc, char *argv[])
{
/* MUTEX */
	pthread_mutex_init(&mux, NULL);

/*SIGNALS*/
	signal(SIGUSR1,tratamento);
	
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
     listen(sockfd,1000); /* 1000 means???*/
     clilen = sizeof(cli_addr);
     
	pthread_create(servadmin_t, NULL, servadmin, NULL);
	threadpool(); /* creates threadpool */
	 
	 sem_init(&sem_fifo_used, 0, 0);
	 sem_init(&sem_fifo_free, 0, FIFO_MAX); 
	 
	 while(1){
	 	sem_wait(&sem_fifo_free);
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          perror("ERROR on accept");
        pthread_mutex_lock(&mux);
        /* Entering Critical Region*/

	    queue (&front ,&back, newsockfd);
	    sem_post(&sem_fifo_used);
        
        /* Exiting Critical Region*/
        pthread_mutex_unlock(&mux);
     }
     close(sockfd);
     return 0; 
}
