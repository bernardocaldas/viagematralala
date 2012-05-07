/* Authors:
João Silva
Bernardo Caldas

Description: manages the size of the threadpool according to the flux of clients;

Arguments: this function must receive a pointer to an integer containing the time when the program started;
*/

#include "fifo.h"
#include "pool.h"



/* Esta função deverá estar integrada no .c e .h referente às estruturas de dados*/

void * manager ( void * arg){
	fifo_node * aux_fifo;
	pool_node * aux_pool;
	int * time_fifo;
	int total_fifo = 0;
	int avg_fifo;
	int * time_pool;
	int total_pool = 0;
	int avg_pool;
	int * begin_time;
	int i,j;
	int tol;
	
	begin_time = (int*) arg;
	i = 0;
	j = 0;
	time_fifo = (int*) calloc(MAX_FIFO*sizeof(int));
	time_pool = (int*) calloc(MAX_POOL*sizeof(int)); 
	
	while(1){
		sleep(5); /* doesn't need to work all the time */

		/* acesso ao FIFO sem cuidados; não há problema se os dados não forem exactos - existirão
		tolerãncias para cobrir esses casos */
		aux_fifo = front;
		while(aux_fifo!= NULL){
			time_fifo[i]=aux_fifo->time-(*begin_time); /* calcula a diferença entra o tempo de começo do programa e o tempo de entrada de um novo pedido no FIFO*/
			total_fifo = total_fifo+time_fifo[i];
			i++;
			aux_fifo = aux_fifo->next;
			/* i -> numero de pedidos na fila */
		}
		aux_pool = first_pool_node;
		while(aux_pool!=NULL){
			time_pool[j]=aux_pool->time;
			total_pool = total_pool+time_pool[j];
			j++;
			aux_pool = aux_pool->next;
			/* j -> numero de threads tratadas */
		}
		avg_fifo = total_fifo/i;
		avg_pool = total_pool/j;
		
		if(avg_pool + tol > avg_fifo){
			create_pool_node (&first_pool_node);
			pool_no++;
		}else{
			if(avg_fifo + tol > avg_pool){
				/* Os elementos na fila estão a entrar com um fluxo menor do que aquele a que estão a ser despachados, não é necessário lançar mais threads; se o fluxo ultrapassar determinada tolerância não valerá a pena continuar com tantas threads activas */
				if(pool_no>MIN_POOL){
					remove_pool_node();
					pool_no--;
					/*pool_no em principio nao precisa de mutex */
				}
			}
		}
	}
	pthread_exit (NULL);
}
