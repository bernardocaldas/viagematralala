#include <semaphore.h>

sem_t sem_pool;
int free_cnt;

typedef struct s_pool{
	pthread_t * thread;
	int socket;
	struct s_pool * next;
	int status;
}pool_node;



void remove_list();
void create_pool_node(pool_node ** first);
pool_node * first_free(pool_node * first);
pool_node * create_pool();




