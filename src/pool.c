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
	pthread_mutex_unlock(&poolmux);
	
}

void remove_pool(pool_node ** first){
	pool_node * aux1, *aux2;
	aux1=*first;
	aux2=*first;
	while(aux1!= NULL){
		if(aux1->thread!=NULL)
		{
			printf("Thread %d will die;\n", aux1->thread);
			pthread_cancel(*(aux1->thread));
			pthread_join(*(aux1->thread),NULL);
		}
		aux2=aux1->next;
		free_pool_node(aux1);
		aux1=aux2;
	}

}

void free_pool_node(pool_node * node){
	FreeStack(node->stack);
	free(node->thread);
	free(node);

}

void remove_pool_node(pool_node **first,pool_node * node){
	pool_node * aux = *first;
	if(node==*first)
	{
		*first=node->next;
		free_pool_node(node);
	}
	else if(aux!=NULL){
		while(aux->next!=node)
		{
		aux=aux->next;
		}
		aux->next=node->next;
		free_pool_node(node);
	
	}
	
}

int pool_time_avg(pool_node ** first, int current_time){
	pool_node * aux;
	aux = *first;
	int total = 0;
	
	while(aux!=NULL){
		total = total + (current_time-aux->time);
		aux=aux->next;
	}	
	if(pool_no==0)
		return 0;
		
	return total/active_threads;
}

