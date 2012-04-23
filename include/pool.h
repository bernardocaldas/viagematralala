int free_cnt;

typedef struct s_pool{
	pthread_t * thread;
	int * socket;
	struct s_pool * next;
	int status;
}pool_node;

void remove_list();
pool_node * insert_list(pool_node * first, pthread_t * thread);
pool_node * create_list();


