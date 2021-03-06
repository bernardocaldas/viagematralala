/*Authors:
Bernardo Caldas
João Silva

Arguments: (local)host, port number (usually 2000+ & it must be the same as the server) filename (pre-written orders) ;

Description: please run 'server' first; creates the socket and binds it with the proper address; 

Problems: makefile can't use all flags - errors concerning the existence of some fields in struch hostent; 

*/

/* TODO : funciona com IP e com hostname!??!?*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "protocol.h"

#define MAX_CONNECT 5

int main(int argc, char *argv[])
{
    int sockfd, portno, n, ntemp,aux;
    char ctemp;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    pid_t pid;
    package * tosend, *torecv;
    char delims[3]={' ',';','\n'};
    char * result;
    char buffer[LEN+1];
    char lixo[LEN+1];
    FILE * file;
    int debug = 0;
    int connect_cnt, aux_connect;
    
    tosend = (package*) malloc(sizeof(package));
    torecv = (package*) malloc(sizeof(package));

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
    
    for (connect_cnt = 0; connect_cnt<MAX_CONNECT; connect_cnt++){
    	aux_connect=connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
    	if (aux_connect == 0){
    		printf("Connection sucessful!\n");
    		break;
    	}
        else{
        	perror("ERROR connecting\n");
        	sleep(2);
        }
    }
    
    if(aux_connect != 0){
    	printf("ERROR Maximum connecting attempts exceeded\n");
    	exit(-1);
   	}
	
	if(argc==4){
		file = fopen(argv[3], "r");
		if(file == NULL){
			perror("ERROR opening file\n");
			file = stdin;
		}
	}else{
		file = stdin;
	}


	bzero(buffer,LEN+1);
	while(fgets(buffer,LEN,file)!=NULL){
		result=strtok((char*)buffer,delims);
		while (result != NULL){
			/*printf("result = %s\n", result);*/
			/* ENVIO DE CARACTER*/
			if(sscanf(result, "%d%s", &ntemp, lixo)==1){
				convert_send(ntemp, tosend->data);
				tosend->op = 'D';
			} else {
				/*ENVIO DE INTEIRO*/
				if(sscanf(result, "%c%s", &ctemp, lixo)==1){
				if(ctemp == 'G')
					debug = !debug;
				if(ctemp == 'D') /* O caracter 'D' não deve ser introduzido pelo utilizador*/
					ctemp = 'E'; 
					convert_send(0, tosend->data);
					tosend->op = ctemp;	
				} else {
				/* TODO cliente deve enviar para servidor tudo o que lhe aparece à frente ou deverá existir um sort à partida? */
					break;
				}
			}
			
		
			n = write(sockfd,tosend,sizeof(package));
			if (n < 0) 
			  error("ERROR writing to socket");
		
			if(ctemp == 'K' ){
				printf("Client closing\n");
				exit(0);
			}			  
		
			n = read(sockfd,torecv,sizeof(package));
			if(n<0)
			  error("ERROR reading from socket");
			if (debug == 1){
				if(tosend->op == 'D')
					printf("%c%d=>",tosend->op, ntemp);
				else
					printf("%c=>", tosend->op);
					printf("%c %d\n",torecv->op, convert_recv(torecv->data));
			}
		
			result = strtok(NULL, delims);
		}
		bzero(buffer,256);
    	
    }
    close(sockfd);
    /* CLEANING */
    fclose(file);
    free(server);
    free(tosend);
    free(torecv);
    return 0;
}
