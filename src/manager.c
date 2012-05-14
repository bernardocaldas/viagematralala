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
#include "math.h"

#define MAX_WAIT_TIME 5

/* Esta função deverá estar integrada no .c e .h referente às estruturas de dados*/
void loop_node_create(float times){
	int i;
	for(i=0; i<times; i++){
		if(pool_no<MAX_POOL){
			create_pool_node (&first_pool_node, 0);
			printf("Thread created\n");
		}
	}
}


void * manager ( void * arg){

	int old_cancel_type;
	int current_time;
	float pool_avg, fifo_avg;
	int tol;
	float create_avg = 1;
	float meter;
	while(1){
		pthread_testcancel();
		sleep(1);
		current_time = time(NULL);
		if (fifo_count != 0 && active_threads != 0){
			pthread_mutex_lock(&poolmux);
			pool_avg = pool_time_avg(&first_pool_node, current_time);
			pthread_mutex_unlock(&poolmux);
		
			pthread_mutex_lock(&fifo_mux);
			fifo_avg = fifo_time_avg(&back_server, current_time);
			pthread_mutex_unlock(&fifo_mux);
		
			/*printf("avg pool time: %f avg fifo time %f\n", pool_avg, fifo_avg);*/
			
			/*			
			if(fifo_avg>pool_avg){
				create_avg=1;
			}else{
				if(fifo_avg<5){
					create_avg=2;
				}else{
					if(fifo_avg<10){
						create_avg=3;
					}else{
						create_avg=4;
					}				
				}				
			}
			*/
			meter = fifo_count * pool_avg;
			if(meter<MAX_WAIT_TIME/2){
				create_avg = 0.25;
			}else{
				if(meter<MAX_WAIT_TIME){
					create_avg = 0.5;
				}else{
					create_avg = 1;
				}
			}
			
			/*if the FIFO is overcrowded but the average waiting time does not surpasses the pool average waiting time*/
			loop_node_create(ceilf(create_avg*(float)fifo_count));
		}
	}

	pthread_exit (NULL);
}
