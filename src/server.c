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
#include "yasc.h"



void error(const char *msg)
{
    perror(msg);
    exit(1);
}



int main(int argc, char *argv[])
{
/* THREADS */
	pthread_t * threads;
	targ * arg;
	threads = (pthread_t *) malloc (1*sizeof(pthread_t));
	arg = (targ *) malloc (1*sizeof(targ));

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
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
	 
	 
	 while(1){
     	
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          error("ERROR on accept");
		arg->socket = newsockfd;
		if (pthread_create(threads, NULL, yasc, (void *)arg) != 0) 
		{
			error("ERROR creating thread\n");
			exit(-1);
		}
     }
     close(sockfd);
     return 0; 
}
