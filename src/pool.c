#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"
#include "yasc.h"

pool_node * create_pool(){
	return NULL;
}

void create_pool_node(pool_node ** first, int flag){

	pool_node * new;
	pool_node * aux;
	new = (pool_node *) malloc (sizeof(pool_node));
	new->thread = (pthread_t*) malloc (sizeof(pthread_t));
	pthread_create(new->thread, NULL, yasc, (void*)new);
	
	
	new->time = 0;
	new->next = NULL;
	new->socket = 0;
	new->stack=NULL;
	new->flag = flag;
	pthread_mutex_init(&(new->stackmux),NULL);
	pthread_mutex_init(&(new->timemux),NULL);
	pthread_mutex_lock(&poolmux);
	aux = *first;
	if(aux!= NULL){
		while(aux->next != NULL){ 
			aux=aux->next;
		}
		aux->next=new;
	}else{
		*first = new;
	}
	pool_no++;
	pthread_mutex_unlock(&poolmux);
	
}

void remove_pool(pool_node ** first){
	pool_node * aux1, *aux2;
	aux1=*first;
	aux2=*first;
	while(aux1!= NULL){
		if(aux1->thread!=NULL)
		{
			/*printf("Thread %d will die;\n", aux1->thread);*/
			aux2=aux1->next;
			pthread_mutex_unlock(&poolmux);/* A função de terminação da thread a ser cancelada invoca remove_pool_node(), que obtém ela própria o lock*/
			pthread_cancel(*(aux1->thread));
			pthread_join(*(aux1->thread),NULL);
			pthread_mutex_lock(&poolmux);
		}
		aux1=aux2;
	}

}


void remove_pool_node(pool_node **first,pool_node * node){ /*poolmux tem de chegar desbloqueado a esta função*/
	pthread_mutex_lock(&poolmux);
	pool_node * aux = *first;
	if(node==*first)
	{
		(*first)=node->next;
		free(node->thread);
		free(node);	
	}
	else if(aux!=NULL){
		while(aux->next!=node)
		{
		aux=aux->next;
		}
		aux->next=node->next;
		free(node->thread);
		free(node);	
	}
	pool_no--;
	pthread_mutex_unlock(&poolmux);
}

float pool_time_avg(pool_node ** first, int current_time){
	pool_node * aux;
	aux = *first;
	float total = 0;
	
	while(aux!=NULL){
		pthread_mutex_lock(&(aux->timemux));
		if(aux->active == 1){
			total = total + (current_time-aux->time);
		} else {
			total = total + aux->time;
		}
		pthread_mutex_unlock(&(aux->timemux));
		aux=aux->next;
	}	
	if(pool_no==0){
		return 0;
	}
		
	return total/pool_no;
}

/* 
FUNCTION: display_client_info

DESCRIPTION:
This is a subfunction of the server administration tool that goes through all the threads in the 'pool' and prints their hostname, IP and current Stack.
*/

void display_client_info(pool_node * first){
	pool_node *aux;
	struct sockaddr_in sockinfo;
	socklen_t addrlen;
	
	aux=first;
	addrlen=sizeof(sockinfo);
	printf("Clients Info:\n");
	pthread_mutex_lock(&poolmux);
	while(aux!= NULL){
		if(aux->socket!=0)
		{
			getpeername(aux->socket,(struct sockaddr *)&sockinfo,&addrlen);
			printf("Client with IP address %s and the following stack contents:",inet_ntoa(sockinfo.sin_addr));
			pthread_mutex_lock(&(aux->stackmux));
			PrintStack(aux->stack);
			pthread_mutex_unlock(&(aux->stackmux));
		}
		aux=aux->next;
	}
	pthread_mutex_unlock(&poolmux);
}
