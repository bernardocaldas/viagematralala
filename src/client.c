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
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include "protocol.h"
#include "fifo.h"


#define MAX_CONNECT 5 /* defines maximum connection attempts */
#define DEBUG_I 0
#define SOCKET_I 1

sem_t fifo_cnt;
pthread_mutex_t fifo;
fifo_node * front;
fifo_node * back;
int debug;
pthread_t main_t;
pthread_t wr_t;

typedef struct main_clean_s 
{
	FILE ** fp;
	package ** tosend;
}main_clean_s;

typedef struct wr_clean_s 
{
	package ** to_recv;
	item_client ** item;
}wr_clean_s;

typedef struct wr_arg_t
{
	int wr_arg;
}wr_arg_t;


void block_all_signals()
{
	sigset_t new_set;
	sigfillset(&new_set);
	sigprocmask(SIG_SETMASK, &new_set, NULL);	
}

void block_except_sigint()
{
	sigset_t new_set;
	sigfillset(&new_set);
	sigdelset(&new_set, SIGINT);
	sigprocmask(SIG_SETMASK, &new_set, NULL);	
}

void wr_clean(void * arg)
{
	wr_clean_s * aux = (wr_clean_s *) arg;
	free(*(aux->to_recv));
	if(*(aux->item)!=NULL)
	{
		free(*(aux->item));
	}
	pthread_mutex_lock(&fifo);
	FreeFifo(&front);
	pthread_mutex_unlock(&fifo);
}

void sig_handler(){
	printf("\nCTRL-C detected\n");
	if(wr_t!=-1) /*If wr_t hasn't been created yet*/
	{
		pthread_cancel(wr_t);
		pthread_join(wr_t,NULL);
	}
	pthread_cancel(main_t);
}

void * write_read ( void * arg){
	int n;
	int ntemp;
	char ctemp;
	int sockfd;
	wr_arg_t * aux;
	package * tosend, *torecv; 
	item_client *item;
	int old_cancel_type;
	wr_clean_s aux_clean;
	aux_clean.to_recv = &torecv;
	aux_clean.item=&item;
	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
    pthread_cleanup_push(wr_clean,(void*)&aux_clean);
	
	
    torecv = (package*) malloc(sizeof(package));
    
    aux = (wr_arg_t*) arg;
    sockfd = aux->wr_arg;

	while(1){
		sem_wait(&fifo_cnt);
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&old_cancel_type); /*Guarantees that the thread doesn't get canceled while holding the lock*/
		pthread_mutex_lock(&fifo);
		item = (item_client *) dequeue(&front, &back);
		pthread_mutex_unlock(&fifo);
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
		
		tosend = &(item->tosend);
		ctemp = tosend->op;
		
		/* WRITING and READING operations*/
		n = write(sockfd,tosend,sizeof(package));
		if (n <= 0){
		  printf("ERROR writing to socket\n");
		  pthread_cancel(main_t);
		  /*exit(-4);*/
		  pthread_exit(NULL);
		 }
		/* if command 'K' is given the client writes to the socket and then terminates without waiting for an answer*/
		if(ctemp == 'K' ){ 
			printf("Client closing\n");
			pthread_cancel(pthread_self());
		}
	
		if(ctemp == 'G'){
			debug = !debug;
		}
		
		n = read(sockfd,torecv,sizeof(package));
		if(n<= 0){
		  printf("ERROR reading from socket\n");
		  pthread_cancel(main_t);
		  /*exit(-4);*/
		  pthread_exit(NULL);
		}
		if (debug){
			if(tosend->op == 'D'){
				printf("%c%d=>",tosend->op, convert_recv(tosend->data));
			}else{
				printf("%c=>", tosend->op);
			}
			printf("%c %d\n",torecv->op, convert_recv(torecv->data));
		}else{
			if(torecv->op != 'E' && torecv->op != 'I'){
				if(tosend->op == 'T' ||tosend->op == 'P' ||tosend->op == 'R'){
					printf("%d\n", convert_recv(torecv->data));
				}
			}
		}
		pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&old_cancel_type); /*Guarantees that the thread isn't canceled while item is being freed, which would result in wr_clean trying to free item again*/
		free(item);
		item=NULL;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);		
	}
	pthread_cleanup_pop(0);
}


void send2fifo(package * tosend, int end_operand){
	int i;
	item_client * item;
	
	for(i=0; i<=end_operand; i++){
		item = (item_client*)malloc(1*sizeof(item_client));
		item->tosend = *tosend;

		pthread_mutex_lock(&fifo);
		queue(&front, &back, item);
		pthread_mutex_unlock(&fifo);
		sem_post(&fifo_cnt);
		tosend->op = 'R';	
	}
}

void main_clean (void * arg){
/* CLEANING */
	main_clean_s * aux = (main_clean_s *)arg;
    if(*(aux->fp)!= stdin && *(aux->fp)!= NULL){
    	fclose(*(aux->fp));
    }
    if(*(aux->tosend)!=NULL)
    	free(*(aux->tosend));
}

int main(int argc, char *argv[])
{
	/*Server*/
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int connect_cnt, aux_connect;
	char file_path [LEN+1];
    /*Communication*/
    char delims[3]={' ','\n'};
    char * result;
    char buffer[LEN+1];
    char lixo[LEN+1];
    FILE * file;
    char ctemp, aux_char;
    int n, aux;
    long long int ntemp;
    package * tosend;
    int end_operand = 0;
    int i;
	struct sigaction sact;
	sigset_t new_set;
    /*Flags*/
    int init = 0;
    int write_enable = 0;
    /*Fifo*/
    pthread_mutex_init(&fifo, NULL);
    create_fifo ( &front, &back);
    sem_init (&fifo_cnt, 0,0);
    int wr_arg;
	/*Signals*/
	sigset_t set;
	
	wr_t=-1;
    main_t=pthread_self();
    int old_cancel_type;
    wr_arg_t arg;
    main_clean_s aux_clean;

    aux_clean.fp=&file;
    aux_clean.tosend=&tosend;	

	debug = 0;

	/* CONECTION WITH SERVER */
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(-1);
    }
	if(argc > 4){
		fprintf(stderr, "usage %s hostname port txtfilename\n", argv[0]);
	}
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
		exit(-5);
	}
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-2);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
    pthread_cleanup_push(main_clean,(void*)&aux_clean);
    
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
		strcpy(file_path,"txt/");
		file = fopen(strcat((char*)file_path,argv[3]), "r");
		if(file == NULL){
			perror("ERROR opening file\n");
			file = stdin;
		}
	}else{
		file = stdin;
	}

	/* COMMUNICATION CYCLE */
	sigfillset (&set);
    sigprocmask(SIG_BLOCK, &set, NULL);

	printf("To initialize communication session press 'I'\nTo terminate current session press 'K'\n");
    tosend = (package*) malloc(sizeof(package));
	arg.wr_arg = sockfd;
	pthread_create(&wr_t, NULL, write_read, (void *) &arg);
	
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
	sact.sa_handler = sig_handler;
	sigaction(SIGINT, &sact, NULL);
	
	block_except_sigint();	
	
	bzero(buffer,LEN+1);
	while(fgets(buffer,LEN,file)!=NULL){
		result=strtok((char*)buffer,delims);
		while (result != NULL){
			write_enable = 0;
			/* INITIALIZING SESSION */
			if(init == 0){
				if(sscanf(result,"%c%s", &ctemp, lixo)==1){
					/*Client closes without waiting for server response*/
					if(ctemp == 'K'){
						/*if 'K' is inserted before the session is initialized the server must be warned. So the wr_t must be created to perform the communication operations*/
						block_all_signals();
						printf("Client closing\n");
						tosend->op = 'K';
						convert_send(0, tosend->data);				
						send2fifo(tosend, end_operand);
						pthread_join(wr_t, NULL);
						close(sockfd);
						exit(0);
					}
					if(ctemp == 'G'){
						debug = !debug;
					}
					if(ctemp == 'I'){
						printf("Communication session has been initialized; to enter debug mode press 'G'\n");
						init = 1;

						/* to minimize the use of global variables the socket file descriptor and the debug flag will be sent via argument to the write_read thread */
						
					}else{
						printf("WARNING please press 'I' to initialize the session\n");
					}
				}else{
		   			printf("WARNING please press 'I' to initialize the session\n");
				}
			}
			if(init==1){
				end_operand = 0;
				/* SENDING DATA*/
				if(sscanf(result, "%lld%s", &ntemp, lixo)==1){

					if(ntemp>INT_MAX){
						printf("ERROR: Overflow\n");
					}else{
						convert_send(ntemp, tosend->data);
						tosend->op = 'D';
						write_enable = 1;
					}
				} else {
					/*SENDING OPERAND*/
					if(sscanf(result, "%c%s", &ctemp, lixo)==1){
						/* the only characters accepted by the client and written in the socket are the ones defined in the protocol*/
						if(ctemp == 'I'|| ctemp == 'P'|| ctemp == 'R'|| ctemp == 'T'|| ctemp == 'K' || ctemp == 'G'||ctemp == '+'||ctemp == '-'||ctemp == '*'||ctemp == '%'||ctemp == '/' || ctemp == ';'){
							if(ctemp == ';'){
								ctemp = 'R';
							}
							else if(ctemp == 'K')
							{
								block_all_signals();
								printf("Client closing\n");
								tosend->op = 'K';
								convert_send(0, tosend->data);
								send2fifo(tosend, end_operand);
								
								pthread_join(wr_t, NULL);
								close(sockfd);
								exit(0);
							}
							convert_send(0, tosend->data);
							tosend->op = ctemp;	
							write_enable = 1;
						}else{
							printf("WARNING please insert a valid command\n");
						}
					}else{
					 	if(sscanf(result, "%c%c%s",&ctemp, &aux_char,lixo)==2){
					 		/* the result is not printed if the command given is the debug command*/
							if(aux_char == ';'){
								convert_send(0, tosend->data);
								tosend->op = ctemp;
								end_operand = 1;
								write_enable = 1;
							}
						}else{
							printf("WARNING please insert a valid command\n");
						}
					}

				}
				if(write_enable == 1){
					block_all_signals();					
					send2fifo(tosend, end_operand);
					block_except_sigint();
				}
			}
			result = strtok(NULL, delims);
		}
		bzero(buffer,256);
    }
    /* functions used specially when reading a file: all the data has been inputed however the server hasn't been able to respond accordingly, so a pthread_join has to be used. The end of the wr_t is forced by sending a 'K'*/
	block_all_signals();
	tosend->op = 'K';	
    convert_send(0, tosend->data);
	send2fifo(tosend, end_operand);
    pthread_join(wr_t, NULL);

    close(sockfd);
    /* CLEANING */
    pthread_cleanup_pop(0);
    return 0;
}
