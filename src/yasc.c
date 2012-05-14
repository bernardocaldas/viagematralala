/*
YASC [Yet Another Simple Calculator]

Programação de Sistemas
2011/2012

Authors:
Bernardo Caldas
João Silva
*/
#include "stack.h"
#include "yasc.h"
#include "protocol.h"
#include "pool.h"
#include "fifo.h"
#include <unistd.h>
#include <limits.h>

#define WAIT_TIME 30

/* auxiliar function related to the termination of the server */
void treatment_kill(void * arg)
{
	pool_node * self;
	self= (pool_node *) arg;
	
	pthread_mutex_lock(&(self->timemux));
	if(self->active==1){
		active_threads--;
	}
	pthread_mutex_unlock(&(self->timemux));
	remove_pool_node(&first_pool_node,self);

}
/*
ARGUMENTS: 
arg - contains a 'pool_node' structure;

DESCRIPTION:
This function is called by threads placed in the pool. The threads will be blocked by the semaphore as soon as they are called. The semaphore wait function depends on whether the threads are permanent or not. Non-permanent threads have a timeout after which they will die. After the Stack is initialized through the 'I' command the calculator will be available.
If the client decides to abandon the session this thread will block until a new request appears (or a timeout is reached);

*/

void * yasc (void * arg)
{	
	/*Fifo*/
	int newsockfd;
	item_server * item;
	/*Thread*/
	pool_node * self;
    struct timespec timer;
    /*Calculator*/
	Stack * stack;
	char ctemp, csend;
	int itemp, nsend, depth, opResult;
	long long int opResult_temp, opA, opB;
	int n;
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
				printf("Thread with ID %d will die \n",(int)pthread_self());
				pthread_exit(NULL);
		}
		}else{
			/*PERMANENT THREAD*/
			/*The thread can't be canceled while holding the lock, or else it would be impossible to clean the queue (and an error would be thrown if someone tried to lock it)*/
			sem_wait(&sem_fifo_used);
		}
		pthread_mutex_lock(&fifo_mux);
		/* Entering Critical Region */
		item =(item_server*) dequeue(&front_server,&back_server);
		sem_post(&sem_fifo_free);
		fifo_count--;
		active_threads++;
		/* Exiting Critcal Region */
		pthread_mutex_unlock(&fifo_mux);
		

		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
		
		newsockfd = item->socket;
		self->socket = item->socket;
		
		pthread_mutex_lock(&(self->timemux));
			self->time = time(NULL);
			self->active = 1;
		pthread_mutex_unlock(&(self->timemux));
	
		
		torecv = (package*) malloc(sizeof(package));
		tosend = (package*) malloc(sizeof(package));
	
		/* INITIALIZATION OF THE COMMUNICATION SESSION*/
		do{
			n = read(newsockfd,torecv,sizeof(package));
			if (n <= 0){
				printf("ERROR reading from socket\n");
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
				if (n <= 0) {
					printf("ERROR writing to socket");
					goto out;
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
				csend = 'E';
			}
			else{
				if(depth==1){
					csend = 'E';
				}
				else{
					switch(ctemp){
					/* SOMA*/
					case '+':
						pthread_mutex_lock(&(self->stackmux));
						opA=(long long int)PopStack(&stack);
						opB=(long long int)PopStack(&stack);
						opResult_temp=opA+opB;
						if(opResult_temp > INT_MAX){
							printf("ERROR: Overflow\n");
							FreeStack(&stack);
							csend = 'I';
						}else{
							opResult = opResult_temp;
							PushStack(&stack,opResult);
							printf("%lld+%lld=%d\n",opA,opB,opResult);
						}
						pthread_mutex_unlock(&(self->stackmux));
						break;
					/*SUBTRAÇÃO*/
					case '-':
						pthread_mutex_lock(&(self->stackmux));
						opA=(long long int)PopStack(&stack);
						opB=(long long int)PopStack(&stack);
						opResult_temp=opA-opB;
						if(opResult_temp<-INT_MAX){
							printf("ERROR: Overflow\n");
							FreeStack(&stack);
							csend='I';
						}else{
							opResult = opResult_temp;
							PushStack(&stack,opResult);
							printf("%lld-%lld=%d\n",opA,opB,opResult);
						}
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case '*':
						pthread_mutex_lock(&(self->stackmux));
						opA=(long long int)PopStack(&stack);
						opB=(long long int)PopStack(&stack);
						opResult_temp=opA*opB;
						if(opResult_temp > INT_MAX){
							printf("ERROR: Overflow\n");
							FreeStack(&stack);
							csend = 'I';
						}else{
							opResult = opResult_temp;
							PushStack(&stack,opResult);
							printf("%lld*%lld=%d\n",opA,opB,opResult);
						}
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case '/':
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						if(opB==0){
							printf("ERROR: divide by zero\n");
							FreeStack(&stack);
							csend = 'I';
						} else{
							opResult=opA/opB;
							PushStack(&stack,opResult);
						}
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case '%':
						pthread_mutex_lock(&(self->stackmux));
						opA=PopStack(&stack);
						opB=PopStack(&stack);
						opResult=opA%opB;
						if(opB==0){
							FreeStack(&stack);
							csend = 'I';
						} else{
							PushStack(&stack,opResult);
						}
						pthread_mutex_unlock(&(self->stackmux));
						break;
					}
					if(csend != 'E'&& csend!='I'){
						csend = 'V';
					}
					nsend = 0;
				
				}
			}
			}else{
				/* COMMANDS*/
				switch(ctemp){
					case 'I':
						pthread_mutex_lock(&(self->stackmux));
						FreeStack(&stack);
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case 'D':
						pthread_mutex_lock(&(self->stackmux));
						PushStack(&stack, itemp);
						pthread_mutex_unlock(&(self->stackmux));
						break;
					case 'P':
						pthread_mutex_lock(&(self->stackmux));
						depth = DepthStack(&stack);
						pthread_mutex_unlock(&(self->stackmux));
						if(depth == 0){
							csend = 'E';
						}else{
							nsend = depth;
						}
						break;
					case 'R':
						pthread_mutex_lock(&(self->stackmux));
						if(DepthStack(&stack)==1){
							nsend = PopStack(&stack);
						}else{
							csend = 'E';
						}	
						pthread_mutex_unlock(&(self->stackmux));
						break;						
					case 'T':
						pthread_mutex_lock(&(self->stackmux));
						if(DepthStack(&stack) == 0){
							csend = 'E';
						}
						else{
							nsend = PopStack(&stack);
							PushStack(&stack, nsend);
						}
					    pthread_mutex_unlock(&(self->stackmux));
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
				if(csend != 'E'&&csend != 'I'){
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
			printf("ERROR writing to socket\n");
			break;
		}

		n = read(newsockfd,torecv,sizeof(package));
		if (n <= 0) {
			printf("ERROR reading from socket\n");
			break;
		}
		ctemp = torecv->op;
		itemp = convert_recv(torecv->data);
		csend = '\0';
		nsend = 0;
	}
	
	out:
		close(newsockfd);
		self->socket = 0;
    	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&old_cancel_type);
		pthread_mutex_lock(&(self->timemux));
		self->time =time(NULL)-self->time;
		self->active = 0; 
		active_threads--;
		pthread_mutex_unlock(&(self->timemux));	
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);	
		/* CLEANING */
		pthread_mutex_lock(&(self->stackmux));
		FreeStack(&stack);
		pthread_mutex_unlock(&(self->stackmux));

	}
	pthread_cleanup_pop(0);
	return NULL;
}

