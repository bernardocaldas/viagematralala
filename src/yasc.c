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
	int itemp,opA,opB,opResult, top;
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
				/* Esta operação é realizada para que não se repitam as verificações relativas à pilha vazia e à presença de um único operando; solução mais elegante precisa-se*/
				if(ctemp=='+' || ctemp=='-' || ctemp=='/'||ctemp=='%'||ctemp=='*'){
				if(EmptyStack(&stack) == 0)
				{
					printf("Pilha vazia: Operação inválida\n");
				}
				else{
					if(DepthStack(&stack)==1){
						printf("Apenas um elemento: Operação inválida\n");
					}
					else{
						switch(ctemp){
						/* SOMA*/
						case '+':
							printf("Apanhei uma soma\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							opResult=opA+opB;
							PushStack(&stack,opResult);
							printf("%d+%d=%d\n",opA,opB,opResult);
							break;
						/*SUBTRAÇÃO*/
						case '-':
							printf("Apanhei uma subtração\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							opResult=opA-opB;
							PushStack(&stack,opResult);
							printf("%d-%d=%d\n",opA,opB,opResult);
							break;
						case '*':
							printf("Apanhei uma multiplicação\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							opResult=opA*opB;
							PushStack(&stack,opResult);
							printf("%d*%d=%d\n",opA,opB,opResult);
							break;
						case '/':
							printf("Apanhei uma divisão\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							if(opB==0){
								printf("ERRO: divisão por zero\n");
							} else{
							opResult=opA/opB;
							PushStack(&stack,opResult);
							printf("%d/%d=%d\n",opA,opB,opResult);
							}
							break;
						case '%':
							printf("Apanhei um módulo\n");
							opA=PopStack(&stack);
							opB=PopStack(&stack);
							opResult=opA%opB;
							PushStack(&stack,opResult);
							printf("%d mod %d=%d\n",opA,opB,opResult);
							break;
						}
					}
				}
				}else{
					
					switch(ctemp){
						case 'I':
							if(EmptyStack(&stack)!=0){
							while(stack!=NULL)
								PopStack(&stack);
							}
							printf("A pilha está agora vazia\n");
							break;
						case 'D':
							/* so quando o protocolo estiver correcto*/
							break;
						case 'P':
							if(EmptyStack(&stack) == 0){
								printf("A pilha está vazia\n");
							}else{
								printf("A pilha tem %d elementos\n", DepthStack(&stack));
							}
							break;
						case 'R':
							/*não estou certo de que seja isto o pretendido*/
							if(DepthStack(&stack)==1){
								printf("O resultado é: %d\n", PopStack(&stack));
							}else{
								printf("Demasiados elementos na pilha\n");
							}	
							break;						
						case 'T':
							if(EmptyStack(&stack) == 0){
								printf("A pilha está vazia\n");
							}
							else{
								top = PopStack(&stack);
								printf("Topo da pilha: %d\n", top);
								PushStack(&stack, top);
							}
							break;
						default:
							printf("Caracter sem função atribuída. Introduza um caracter válido\n");
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
