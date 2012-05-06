#include "fifo.h"
#include "pool.h"

/* Esta função deverá estar integrada no .c e .h referente às estruturas de dados*/

void * manager ( void * arg){
	fifo_node * aux_fifo;
	pool_node * aux_pool;
	int * time_fifo;
	int * time_pool;
	
	int i,j;
	
	i = 0;
	j = 0;
	time_fifo = (int*) calloc(MAX_FIFO*sizeof(int));
	time_pool = (int*) calloc(POOL_NO*sizeof(int)); /*este POOL_NO terá que ser redifinido quando a POOL se alterar*/
	while(1){
		sleep(5); /* doesn't need to work all the time */

		/* acesso ao FIFO sem cuidados; não há problema se os dados não forem exactos - existirão
		tolerãncias para cobrir esses casos */
		aux_fifo = front;
		while(aux_fifo!= NULL){
			time_fifo[i]=aux_fifo->time;
			i++;
			aux_fifo = aux_fifo->next;
		}
		aux_pool = first_pool_node;
		while(aux_pool!=NULL){
			time_pool[j]=aux_pool->time;
			j++;
			aux_pool = aux_pool->next;
		}
	
	}
	pthread_exit (NULL);
}
