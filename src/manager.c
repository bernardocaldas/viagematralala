/*
Manager

Programação de Sistemas
2011/2012

Authors:
67557 ~ Bernardo Caldas
67636 ~ João Silva
 
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "fifo.h"
#include "pool.h"
#include "math.h"

#define MAX_WAIT_TIME 5

/* FUNCTION: loop_node_create

DESCRIPTION: loop function that creates as many nodes as the ones specified in times. Although times is a float it has been ceiled beforehand. The threads created may never exceed MAX_POOL defined in pool.h

*/
void loop_node_create(float times){
	int i;
	for(i=0; i<times; i++){
		if(pool_no<MAX_POOL){
			create_pool_node (&first_pool_node, 0);
			printf("Thread created\n");
		}
	}
}
/*
Description: manages the size of the threadpool according to the flux of clients; it only controls the creation of new pool_nodes. Non permanent pool_nodes will terminate after WAIT_TIME

Arguments: this function must receive a pointer to an integer containing the time when the program started;
*/

void * manager ( void * arg){

	int current_time;
	float pool_avg;
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
		
		
			/*printf("avg pool time: %f avg fifo time %f\n", pool_avg, fifo_avg);*/
			
			
			meter = fifo_count * pool_avg/pool_no;
			
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
