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
	Stack * stack = CreateStack();
	char ctemp, csend;
	int itemp,opA,opB,opResult, top, nsend;
	char * result;
	char delims[2]={' ',';'};
	char buffer[256];
	char msg[256];
    int newsockfd, n;

    package * torecv, *tosend;
    fifo_node * current;
    
    while(1){
    
		sem_wait(&sem_fifo);
		
		pthread_mutex_lock(&mux);
		/* Entrada Regiao Critica */
		current = dequeue(&front,&back);
		/* Saida Regiao Critica */
		pthread_mutex_unlock(&mux);
		/*SEMAFORO*/
		/* Distribuição para a lista */
		/* função que procura na lista a primeira thread livre. Vai encontrar uma thread livre (dada a condiçao anterior) e colocará no campo int * socket a informação relativa ao novo file descriptor. Em seguida envia um sinal para que a thread acorde */
		
		
		torecv = (package*) malloc(sizeof(package));
		tosend = (package*) malloc(sizeof(package));

		newsockfd = current->socket;
	
		/* leitura e resposta inicial*/
		n = read(newsockfd,torecv,sizeof(package));
		if (n <= 0){
			perror("ERROR reading from socket");
			goto out;
		}
	
		ctemp = torecv->op;
		itemp = ntohl(torecv->data);
		csend = '\0';
		nsend = 0;

		while(ctemp != 'K')
		{
			/* Esta operação é realizada para que não se repitam as verificações relativas à pilha vazia e à presença de um único operando; solução mais elegante precisa-se*/
			if(ctemp=='+' || ctemp=='-' || ctemp=='/'||ctemp=='%'||ctemp=='*'){
			if(EmptyStack(&stack) == 0)
			{
				printf("Pilha vazia: Operação inválida\n");
				csend = 'E';
			}
			else{
				if(DepthStack(&stack)==1){
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
						if(EmptyStack(&stack)!=0){
						while(stack!=NULL)
							PopStack(&stack);
						}
						printf("A pilha está agora vazia\n");
						break;
					case 'D':
						printf("Apanhei um número %d\n", itemp);
						PushStack(&stack, itemp);
						break;
					case 'P':
						if(EmptyStack(&stack) == 0){
							printf("A pilha está vazia\n");
							csend = 'E';
						}else{
							nsend = DepthStack(&stack);
							printf("A pilha tem %d elementos\n", nsend);
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
			goto out;
		}

		n = read(newsockfd,torecv,sizeof(package));
		if (n <= 0) {
			perror("ERROR reading from socket");
			goto out;
		}
		ctemp = torecv->op;
		itemp = ntohl(torecv->data);
		csend = '\0';
		nsend = 0;
	}
	
	out:
		printf("Thread vai fechar o seu socket\n");
		close(newsockfd);
		/* CLEANING */
		FreeStack(&stack);
	}
}
