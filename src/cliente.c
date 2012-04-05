/*Author: Allmighty Internet

Arguments: (local)host, port number (usually 2000+ & it must be the same as the server) ;

Description: please run 'server' first; creates the socket and binds it with the proper address; forking so that cliente_son can write and cliente_father can read - their work is completely independent;

Problems: makefile can't use all flags - errors concerning the existence of some fields in struch hostent; the work done by the cliente_father and cliente_son shouldn't be completely independent - must create a way for them to communicate properly so that only when we are sure that the server got the message will we ask for a new message to be inserted;

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pid_t pid;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    pid = fork();
    while(1){
      if(pid<0){
	error("ERROR on forking\n");
      }
      /*Processo filho*/
      if(pid==0){
	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
	  error("ERROR writing to socket");

      }
      /*Processo pai*/
      else{
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if(n<0)
	  error("ERROR reading from socket");
	printf("PAI(leitura):%s\n",buffer);
      }

    }
    close(sockfd);
    return 0;
}
