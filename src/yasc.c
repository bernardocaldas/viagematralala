/* Yet Another Simple Calculator

A função de que consiste o programa é invocada por server.c para servir como código das threads criadas


*/
#include "stack.h"
#include "yasc.h"

#define MAXLINHA 20


void * yasc (void * arg)
{	
	Stack * stack = CreateStack();
	char ctemp;
	int itemp,opA,opB,opResult;
	char * result;
	char delims[2]={' ',';'};
	char buffer[256];
	char msg[256];
    int newsockfd, n;
    int * status;
    
    status =(int*)malloc(1*sizeof(int));
    
    targ * argument;
	bzero(buffer,256);
	argument = (targ*) arg;
	newsockfd = argument->socket;
	
	/* leitura e resposta inicial*/
	n = read(newsockfd,buffer,255);
	if (n < 0){
		error("ERROR reading from socket");
		exit(-1);
	}
	strcpy(msg, "Message Received\n");
	n = write(newsockfd, msg, 255);
	if (n < 0){
		error("ERROR writing to socket");
		exit(-2);
	}

	while(strcmp((char*)buffer, "K\n")!=0)
	{
		result=strtok((char*)buffer,delims);
		while(result!=NULL)
		{
			if(sscanf(result,"%d",&itemp)==1)
			{
				printf("Apanhei o numero %d\n",itemp);
				PushStack(&stack, itemp);
			}
			else
			{
				if(sscanf(result,"%c",&ctemp)==1){
				if(EmptyStack(&stack) == 0)
				{
					printf("Pilha vazia: Operação inválida\n");
				}
				else{
					if(OneOp(&stack)==0){
						printf("Apenas um elemento: Operação inválida\n");
					}
					else{
						if(ctemp=='+'){
							printf("Apanhei uma soma\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							opResult=opA+opB;
							PushStack(&stack,opResult);
							printf("%d+%d=%d\n",opA,opB,opResult);
						}
					}
				}
				}
			}
			result=strtok(NULL,delims);
		}
		if (result == NULL){ /* talvez esta verificação seja redundante*/
			bzero(buffer,256);
			n = read(newsockfd,buffer,255);
			if (n < 0) {
				error("ERROR reading from socket");
				exit(-1);
			}
			n = write(newsockfd, msg, 255);
			if (n < 0) {
				error("ERROR writing to socket");
				exit(-2);
			}
		}
	}
	
	printf("Thread vai fechar o seu socket\n");
	close(newsockfd);
	*status = 0;
	pthread_exit(&status);
}
