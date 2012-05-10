/*
Client

Programação de Sistemas
2011/2012

Authors:
67557 ~ Bernardo Caldas
67636 ~ João Silva

ARGUMENTS: 
serverhost, portnumber (usually 2000+ & it must be the same as the server), filename (pre-written orders) ;

DESCRIPTION: 
Creates the socket and binds it with the proper address. Then a connection attempt is made: if the connection does not succeed the client will retry to connect again until the limit MAX_CONNECT is achieved; if the connection succeeds the program will ask for the initialization variable 'I'. Until such criterion is met there is no writing to the socket.

RETURNS:
- 0 by default (only reached if fgets == NULL);
- 1 if the user inserts command K;

- -1 the arguments usage wasn't correct;
- -2 the specified host does not exist;
- -3 maximum connecting attempts;
- -4 error writing to or reading from socket;

Problems: makefile can't use all flags - errors concerning the existence of some fields in struct hostent; 
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


#define MAX_CONNECT 5 /* defines maximum connection attempts */

void clean ( struct hostent * server, package * tosend, package * torecv, FILE * file){
/* CLEANING */
    if(file!= stdin){
    	fclose(file);
    }
    free(tosend);
    free(torecv);
}

int main(int argc, char *argv[])
{
	/*Server*/
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int connect_cnt, aux_connect;
    /*Communication*/
    package * tosend, *torecv;
    char delims[3]={' ',';','\n'};
    char * result;
    char buffer[LEN+1];
    char lixo[LEN+1];
    FILE * file;
    char ctemp;
    int n, ntemp, aux;
    /*Flags*/
    int debug = 0;
    int init = 0;
	
	/* CONECTION WITH SERVER */
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(-1);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-2);
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
    	exit(-3);
   	}
	
	/* ORDERS FROM FILE */
	if(argc==4){
		file = fopen(argv[3], "r");
		if(file == NULL){
			perror("ERROR opening file\n");
			file = stdin;
		}
	}else{
		file = stdin;
	}

	/* COMMUNICATION CYCLE */
	printf("To initialize communication session press 'I'\nTo terminate current session press 'K'\n");
	tosend = (package*) malloc(sizeof(package));
    torecv = (package*) malloc(sizeof(package));
    
	bzero(buffer,LEN+1);
	while(fgets(buffer,LEN,file)!=NULL){
		result=strtok((char*)buffer,delims);
		while (result != NULL){
			/* INITIALIZING SESSION */
			if(init == 0){
				if(sscanf(result,"%c%s", &ctemp, lixo)==1){
					if(ctemp == 'K' ){
						printf("Client closing\n");
						exit(0);
					}
					if(ctemp == 'G')
						debug = !debug;
					if(ctemp == 'I'){
						printf("Communication session has been initialized; to enter debug mode press 'G'\n");
						init = 1;
					}else{
						printf("WARNING please press 'I' to initialize the session\n");
					}
				}else{
		   			printf("WARNING please press 'I' to initialize the session\n");
				}
			}
			if(init==1){
				/* SENDING OPERAND CHAR*/
				if(sscanf(result, "%d%s", &ntemp, lixo)==1){
					convert_send(ntemp, tosend->data);
					tosend->op = 'D';
				} else {
					/*SENDING DATA*/
					if(sscanf(result, "%c%s", &ctemp, lixo)==1){
						if(ctemp == 'G'){
							debug = !debug;
						}
						if(ctemp == 'D'){ /* O caracter 'D' não deve ser introduzido pelo utilizador*/
							ctemp = 'E';
						} 
						convert_send(0, tosend->data);
						tosend->op = ctemp;	
					} else {
					/* TODO cliente deve enviar para servidor tudo o que lhe aparece à frente ou deverá existir um sort à partida para além do 'lixo' que já não é comtemplado */
						break;
					}
				}
				/* WRITING and READING operations*/
				n = write(sockfd,tosend,sizeof(package));
				if (n <= 0){
				  perror("ERROR writing to socket");
				  clean (server,tosend, torecv, file);
				  exit(-4);
				 }
				/* if command 'K' is given the client closes without waiting for an answer*/
				if(ctemp == 'K' ){ 
					printf("Client closing\n");
					return 1;
				}
				
				n = read(sockfd,torecv,sizeof(package));
				if(n<= 0){
				  perror("ERROR reading from socket");
				  clean (server,tosend, torecv, file);
				  exit(-4);
				}
				if (debug == 1){
					if(tosend->op == 'D')
						printf("%c%d=>",tosend->op, ntemp);
					else
						printf("%c=>", tosend->op);
						printf("%c %d\n",torecv->op, convert_recv(torecv->data));
				}
				
			}
			result = strtok(NULL, delims);
		}
		bzero(buffer,256);
    }
    close(sockfd);
    /* CLEANING */
    clean (server,tosend, torecv, file);
    return 0;
}
