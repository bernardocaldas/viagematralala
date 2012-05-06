#define POOL_NO 10

int free_cnt;

typedef struct s_pool{
	pthread_t * thread;
	struct s_pool * next;
	int time;
}pool_node;

pool_node * first_pool_node;


void remove_list();
void create_pool_node(pool_node ** first);
pool_node * first_free(pool_node * first);
pool_node * create_pool();




