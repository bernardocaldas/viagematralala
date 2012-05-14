#ifndef POOL_H
#define POOL_H


#include "stack.h"
#include "pthread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define POOL_INIT 1
#define MIN_POOL 1
#define MAX_POOL 1000

int pool_no;
int active_threads;

pthread_mutex_t poolmux;
pthread_mutex_t active_thread_mux;

typedef struct s_pool{
	pthread_t * thread;
	struct s_pool * next;
	int time;
	int socket;
	int flag;
	Stack ** stack;
	pthread_mutex_t stackmux;
	pthread_mutex_t timemux;
	int active;
}pool_node;

pool_node * first_pool_node;


void remove_pool(pool_node ** first);
void create_pool_node(pool_node ** first, int flag);
pool_node * create_pool();
void free_pool_node(pool_node *);
void remove_pool_node(pool_node **,pool_node *);
float pool_time_avg(pool_node ** first, int current_time);
void display_client_info(pool_node *);
#endif


