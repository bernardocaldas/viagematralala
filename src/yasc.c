/*
YASC [Yet Another Simple Calculator]

Programação de Sistemas
2011/2012

Authors:
Bernardo Caldas
João Silva

ARGUMENTS: 
arg - contains a 'pool_node' structure;

DESCRIPTION:
This function is called by threads placed in the pool. The threads will be blocked by the semaphore as soon as they are called. The semaphore wait function depends on whether the threads are permanent or not. Non-permanent threads have a timeout after which they will die. After the Stack is initialized through the 'I' command the calculator will be available.
If the client decides to abandon the session this thread will block until a new request appears (or a timeout is reached);

*/
#include "stack.h"
#include "yasc.h"
#include "protocol.h"
#include "pool.h"
#include "fifo.h"
#include <unistd.h>
#include <signal.h>

#define WAIT_TIME 10

/* auxiliar function related to the termination of the server */
void treatment_kill(void * arg)
{
	pool_node * self;
	self= (pool_node *) arg;
	FreeStack(self->stack);
	pthread_mutex_lock(&active_thread_mux);
	active_threads--;
	pthread_mutex_unlock(&active_thread_mux);

}


void * yasc (void * arg)
{	
	/*Thread*/
	pool_node * self;
    struct timespec timer;
    /*Calculator*/
	Stack * stack;
	char ctemp, csend;
	int itemp,opA,opB,opResult, top, nsend, depth;
	int newsockfd, n;
    package * torecv, *tosend;

    
    self = (pool_node *)malloc(1*sizeof(pool_node)); /*Esta linha é precisa?*/
    self = (pool_node *)arg;
    /*Cleanup*/
    int old_cancel_type;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
    pthread_cleanup_push(treatment_kill,(void*)self);
    
    while(1){
    	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&old_cancel_type);
    	/*NON-PERMANENT THREAD*/
    	if(self->flag == 0){
    		timer.tv_sec = time(NULL)+WAIT_TIME;
    		timer.tv_nsec = 0;
			if(sem_timedwait(&sem_fifo_used, &timer)==-1){
				printf("Thread will die; sem_out %d\n", n);
				pthread_mutex_lock(&poolmux);
				remove_pool_node(&first_pool_node,self);
				pthread_mutex_unlock(&poolmux);
				pthread_exit(NULL);
		}
	}else{
		/*PERMANENT THREAD*/
		 
		/*The thread can't be canceled while holding the lock, or else it would be impossible to clean the queue (and an error would be thrown if someone tried to lock it)*/
		sem_wait(&sem_fifo_used);
	}
		pthread_mutex_lock(&mux);
		/* Entering Critical Region */
		newsockfd = dequeue(&front,&back);
		sem_post(&sem_fifo_free);
		fifo_count--;
		/* Exiting Critcal Region */
		pthread_mutex_unlock(&mux);
		
		pthread_mutex_lock(&active_thread_mux);
		active_threads++;
		pthread_mutex_unlock(&active_thread_mux);
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
		self->socket = newsockfd;
		self->time = time(NULL);
		
		torecv = (package*) malloc(sizeof(package));
		tosend = (package*) malloc(sizeof(package));
	
		/* INITIALIZATION OF THE COMMUNICATION SESSION*/
		do{
			n = read(newsockfd,torecv,sizeof(package));
			if (n <= 0){
				perror("ERROR reading from socket");
				goto out; /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
			}
			if(torecv->op == 'I'){
			    stack=CreateStack();
    			pthread_mutex_lock(&(self->stackmux));
    			self->stack=&stack;
    			pthread_mutex_unlock(&(self->stackmux));
				break;
			}
			else{
				convert_send(0, tosend->data);
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
		itemp = convert_recv(torecv->data);
		csend = '\0';
		nsend = 0;
		
		/* COMMUNICATION SESSION */
		while(ctemp != 'K')
		{
			/* MATH OPERANDS*/
			if(ctemp=='+' || ctemp=='-' || ctemp=='/'||ctemp=='%'||ctemp=='*'){
			pthread_mutex_lock(&(self->stackmux));
			depth = DepthStack(&stack);
			pthread_mutex_unlock(&(self->stackmux));
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
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						opResult=opA+opB;
						PushStack(&stack,opResult);
						pthread_mutex_unlock(&(self->stackmux));
						printf("%d+%d=%d\n",opA,opB,opResult);
						break;
					/*SUBTRAÇÃO*/
					case '-':
						printf("Apanhei uma subtração\n");
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						opResult=opA-opB;
						PushStack(&stack,opResult);
						pthread_mutex_unlock(&(self->stackmux));
						printf("%d-%d=%d\n",opA,opB,opResult);
						break;
					case '*':
						printf("Apanhei uma multiplicação\n");
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						opResult=opA*opB;
						/*MAX_INT!!!!!!!!!!!!!!!!!!!!!!!!!!*/
						PushStack(&stack,opResult);
						pthread_mutex_unlock(&(self->stackmux));
						printf("%d*%d=%d\n",opA,opB,opResult);
						break;
					case '/':
						printf("Apanhei uma divisão\n");
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						if(opB==0){
							printf("ERRO: divisão por zero\n");
							csend = 'E';
						} else{
						opResult=opA/opB;
						PushStack(&stack,opResult);
						pthread_mutex_unlock(&(self->stackmux));
						printf("%d/%d=%d\n",opA,opB,opResult);
						}
						break;
					case '%':
						printf("Apanhei um módulo\n");
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						opResult=opA%opB;
						PushStack(&stack,opResult);
						pthread_mutex_unlock(&(self->stackmux));
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
				/* COMMANDS*/
				switch(ctemp){
					case 'I':
						pthread_mutex_lock(&(self->stackmux));
						FreeStack(&stack);
						pthread_mutex_unlock(&(self->stackmux));
						printf("A pilha está agora vazia\n");
						break;
					case 'D':
						printf("Apanhei um número %d\n", itemp);
						pthread_mutex_lock(&(self->stackmux));
						PushStack(&stack, itemp);
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case 'P':
						pthread_mutex_lock(&(self->stackmux));
						depth = DepthStack(&stack);
						pthread_mutex_unlock(&(self->stackmux));
						if(depth == 0){
							printf("A pilha está vazia\n");
							csend = 'E';
						}else{
							nsend = depth;
							printf("A pilha tem %d elemento(s)\n", nsend);
						}
						break;
					case 'R':
						pthread_mutex_lock(&(self->stackmux));
						if(DepthStack(&stack)==1){
							nsend = PopStack(&stack);
							pthread_mutex_unlock(&(self->stackmux));
							printf("O resultado é: %d\n", nsend);
						}else{
							printf("Pilha vazia ou com demasiados elementos\n");
							csend = 'E';
						}	
						break;						
					case 'T':
						pthread_mutex_lock(&(self->stackmux));
						if(DepthStack(&stack) == 0){
							pthread_mutex_unlock(&(self->stackmux));
							printf("A pilha está vazia\n");
							csend = 'E';
						}
						else{
							pthread_mutex_lock(&(self->stackmux));
							nsend = PopStack(&stack);
							printf("Topo da pilha: %d\n", nsend);
							PushStack(&stack, nsend);
							pthread_mutex_unlock(&(self->stackmux));
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
		/*WRITING and READING*/
		convert_send(nsend, tosend->data);
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
		itemp = convert_recv(torecv->data);
		csend = '\0';
		nsend = 0;
	}
	
	out:
		printf("Thread will close its socket\n");
		close(newsockfd);
		self->socket = 0;
		/* CLEANING */
		pthread_mutex_lock(&(self->stackmux));
		FreeStack(&stack);
		pthread_mutex_unlock(&(self->stackmux));

	}
	pthread_cleanup_pop(0);
	return NULL;
}

