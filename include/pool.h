#ifndef POOL_H
#define POOL_H


#include "stack.h"
#include "pthread.h"
#define POOL_INIT 1
#define MIN_POOL 1
#define MAX_POOL 3

int pool_no;
int free_cnt;
pthread_mutex_t poolmux;

typedef struct s_pool{
	pthread_t * thread;
	struct s_pool * next;
	int time;
	int socket;
	int flag;
	Stack ** stack;
	pthread_mutex_t stackmux;
}pool_node;

pool_node * first_pool_node;


void remove_pool(pool_node ** first);
void create_pool_node(pool_node ** first, int flag);
pool_node * create_pool();
void remove_pool_node(pool_node *);

#endif


