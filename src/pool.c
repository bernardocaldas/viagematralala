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
	aux = *first;
	new->time = 0;
	new->next = NULL;
	new->socket = 0;
	new->stack=NULL;
	new->flag = flag;
	if(aux!= NULL){
		while(aux->next != NULL){ 
			aux=aux->next;
		}
		aux->next=new;
	}else{
		*first = new;
	}
	
}

void remove_list(){
/* FAAAAAAALTA!!!!!*/

}

void remove_pool_node(){

}

