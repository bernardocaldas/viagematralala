#ifndef POOL_H
#define POOL_H

#define POOL_INIT 10
#define MIN_POOL 10
#define MAX_POOL 20

int pool_no;
int free_cnt;

typedef struct s_pool{
	pthread_t * thread;
	struct s_pool * next;
	int time;
}pool_node;

pool_node * first_pool_node;


void remove_list();
void create_pool_node(pool_node ** first);
pool_node * create_pool();
void remove_pool_node();

#endif


