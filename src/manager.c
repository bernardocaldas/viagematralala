/* Authors:
67636 ~ João Silva
67557 ~ Bernardo Caldas

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

	int old_cancel_type;
	int current_time;
	int pool_avg, fifo_avg;
	int tol;

	while(1){
		pthread_testcancel();
		sleep(1);
		current_time = time(NULL);
		if (fifo_count != 0 && active_threads != 0){
			pthread_mutex_lock(&poolmux);
			pool_avg = pool_time_avg(&first_pool_node, current_time);
			pthread_mutex_unlock(&poolmux);
		
			pthread_mutex_lock(&mux);
			fifo_avg = fifo_time_avg(&back_server, current_time);
			pthread_mutex_unlock(&mux);
		
			printf("avg pool time: %d avg fifo time %d\n", pool_avg, fifo_avg);
		
			/* average waiting time in pool is bigger than the average waiting time in the fifo*/
			if(pool_avg>fifo_avg){
				pthread_mutex_lock(&poolmux);
				create_pool_node (&first_pool_node, 0);
				pool_no++;
				pthread_mutex_unlock(&poolmux);
			} 
			/*if the FIFO is overcrowded but the average waiting time does not surpasses the pool average waiting time*/
			else{
				if(fifo_count > 2/3*MAX_FIFO){
					if(pool_no<MAX_POOL){
						create_pool_node (&first_pool_node, 0);
						pool_no++;
					}
				}
			}
		}
	}

	pthread_exit (NULL);
}

