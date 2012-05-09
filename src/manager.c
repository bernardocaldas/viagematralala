/* Authors:
João Silva
Bernardo Caldas

Description: manages the size of the threadpool according to the flux of clients;

Arguments: this function must receive a pointer to an integer containing the time when the program started;
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "fifo.h"
#include "pool.h"



/* Esta função deverá estar integrada no .c e .h referente às estruturas de dados*/

void * manager ( void * arg){
	
	
	while(1){
		sleep(1);
		if(fifo_count > 2/3*MAX_FIFO)
			if(pool_no<MAX_POOL){
				create_pool_node (&first_pool_node, 0);
				pool_no++;
			}
	}
	pthread_exit (NULL);
}
