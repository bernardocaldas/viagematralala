

pool_node * create_list(){
	return NULL;
}

pool_node * insert_list(pool_node * first, pthread_t * thread){
	pool_node * new;
	pool_node * aux;
	new->socket = NULL;
	new->status = 0; /* zero = FREE */
	new->thread = thread;
	aux = first;
	new->next = NULL;
	if(aux!= NULL){
		while(aux->next != NULL){ 
			aux=aux->next;
		}
		aux->next=new;
	}else{
		first = new;
	}
	return first;
}

void remove_list(){


}
