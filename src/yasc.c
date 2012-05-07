/* Yet Another Simple Calculator

A função de que consiste o programa é invocada por server.c para servir como código das threads criadas


*/
#include "stack.h"
#include "yasc.h"
#include "protocol.h"
#include "pool.h"
#include "fifo.h"

#define MAXLINHA 20


void * yasc (void * arg)
{	
	Stack * stack;
	char ctemp, csend;
	int itemp,opA,opB,opResult, top, nsend, depth;
	char * result;
	char delims[2]={' ',';'};
	char buffer[256];
	char msg[256];
    int newsockfd, n;
    int time_begin, time_end;

    package * torecv, *tosend;
    pool_node * self;
    
    self = (pool_node *)malloc(1*sizeof(pool_node));
    self = (pool_node *)arg;
    
    time_begin = 0;
   
    
    while(1){
    	
    	self->time = time_end-time_begin;
		sem_wait(&sem_fifo_used);
		
		pthread_mutex_lock(&mux);
		/* Entrada Regiao Critica */
		newsockfd = dequeue(&front,&back);
		sem_post(&sem_fifo_free);
		/* Saida Regiao Critica */
		pthread_mutex_unlock(&mux);
		self->socket = newsockfd;
		time_begin = time(NULL);
		
		torecv = (package*) malloc(sizeof(package));
		tosend = (package*) malloc(sizeof(package));
	
		/* leitura e resposta inicial*/
		do{
			n = read(newsockfd,torecv,sizeof(package));
			if (n <= 0){
				perror("ERROR reading from socket");
				break;
			}
			if(torecv->op == 'I')
				break;
			else{
				tosend->data = htonl(0);
				tosend->op = 'E';
				n = write(newsockfd, tosend, sizeof(package));
				if (n < 0) {
					perror("ERROR writing to socket");
					break;
				}
			}
		}
		while(1);
	
		ctemp = torecv->op;
		itemp = ntohl(torecv->data);
		csend = '\0';
		nsend = 0;

		while(ctemp != 'K')
		{
			/* Esta operação é realizada para que não se repitam as verificações relativas à pilha vazia e à presença de um único operando; solução mais elegante precisa-se*/
			if(ctemp=='+' || ctemp=='-' || ctemp=='/'||ctemp=='%'||ctemp=='*'){
			depth = DepthStack(&stack);
			if(depth == 0)
			{
				printf("Pilha vazia: Operação inválida\n");
				csend = 'E';
			}
			else{
				if(depth==1){
					printf("Apenas um elemento: Operação inválida\n");
					csend = 'E';
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
							csend = 'E';
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
					if(csend != 'E'){
						csend = 'V';
						nsend = opResult;
					}else{
						nsend = 0;
					}
				
				}
			}
			}else{
			
				switch(ctemp){
					case 'I':
						FreeStack(&stack);
						printf("A pilha está agora vazia\n");
						break;
					case 'D':
						printf("Apanhei um número %d\n", itemp);
						PushStack(&stack, itemp);
						break;
					case 'P':
						depth = DepthStack(&stack);
						if(depth == 0){
							printf("A pilha está vazia\n");
							csend = 'E';
						}else{
							nsend = depth;
							printf("A pilha tem %d elemento(s)\n", nsend);
						}
						break;
					case 'R':
						/*não estou certo de que seja isto o pretendido*/
						if(DepthStack(&stack)==1){
							nsend = PopStack(&stack);
							printf("O resultado é: %d\n", nsend);
						}else{
							printf("Pilha vazia ou com demasiados elementos\n");
							csend = 'E';
						}	
						break;						
					case 'T':
						if(DepthStack(&stack) == 0){
							printf("A pilha está vazia\n");
							csend = 'E';
						}
						else{
							nsend = PopStack(&stack);
							printf("Topo da pilha: %d\n", nsend);
							PushStack(&stack, nsend);
						}
						break;
					case 'G':
						csend = 'V';
						nsend = 0;
						break;
					default:
						printf("Caracter sem função atribuída. Introduza um caracter válido\n");
						csend = 'E';
						break;
				}
				if(csend != 'E'){
						csend = 'V';
				}else{
						nsend = 0;
				}
			}
	
		tosend->data = htonl(nsend);
		tosend->op = csend;
		n = write(newsockfd, tosend, sizeof(package));
		if (n < 0) {
			perror("ERROR writing to socket");
			break;
		}

		n = read(newsockfd,torecv,sizeof(package));
		if (n <= 0) {
			perror("ERROR reading from socket");
			break;
		}
		ctemp = torecv->op;
		itemp = ntohl(torecv->data);
		csend = '\0';
		nsend = 0;
	}
	
	out:
		printf("Thread vai fechar o seu socket\n");
		close(newsockfd);
		self->socket=0;
		/* CLEANING */
		FreeStack(&stack);
		time_end = time(NULL);
	}
}
