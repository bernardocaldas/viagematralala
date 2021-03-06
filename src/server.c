/*
Server

Programação de Sistemas
2011/2012

Authors:
67557 ~ Bernardo Caldas
67636 ~ João Silva
 
*/

#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "yasc.h"
#include "fifo.h"
#include "pool.h"
#include "protocol.h"
#include "manager.h"

pthread_t mainthread;


void server_cleanup (){

	pthread_mutex_lock(&poolmux);
	remove_pool(&first_pool_node);
	pthread_mutex_unlock(&poolmux);
	
	pthread_mutex_lock(&mux);
	FreeFifo(&front_server);
	pthread_cancel(mainthread);
	pthread_mutex_unlock(&mux);
	
	printf("Cleanup finished. Bye!\n");
	pthread_exit(0);
	
}

/*
FUNCTION: servadmin

DESCRIPTION:
Function that is run by a particular thread that is always available for input via terminal. It accepts two commands:
	- F: closes the server and frees all its memory;
	- M: displays info of the machines attached to the server at the current moment;
*/
void * servadmin (void * arg){
	struct sigaction sact;
	sigset_t new_set;
	char buffer[LEN],lixo[LEN],ctemp;
	
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
	sact.sa_handler = server_cleanup;
	sigaction(SIGINT, &sact, NULL);
	
	sigfillset(&new_set);
	sigdelset(&new_set, SIGINT);
	sigprocmask(SIG_SETMASK, &new_set, NULL);
	
	bzero(buffer,LEN+1);
	printf("Insert command: \n");
	while(fgets(buffer,LEN,stdin)!=NULL){
		if(sscanf(buffer, "%c%s", &ctemp, lixo)==1){
			if(ctemp=='M'){
				display_client_info(first_pool_node);
			}
			else if(ctemp=='F'){
				server_cleanup();
				/*raise(SIGUSR1);*/
				/*TEMP exit(1);*/
			}
			else{
			printf("Please insert valid commands\n");
			}
		} 
		else{
			printf("Please insert valid commands\n");
		}
		printf("Insert command: \n");
	}
	printf("servadmin will terminate\n");
}

/* FUNCTION: threadpool 

DESCRIPTION: 
this function is not associated with any particular thread as it is called only once by the main function in order to initialize the thread_pool; before such measures have been taken the server will be unable to cope with client requests
Since the server administration thread isn't running yet, only one thread is working on the pool, therefore no race conditions exist.
*/
void threadpool (){

	int i;
	pool_no = POOL_INIT;
	first_pool_node = create_pool();
	for(i=0; i< pool_no; i++){
		create_pool_node(&first_pool_node, 1);
	}	
}

void mainthread_kill(void * arg)
{
	pthread_t * manager=(pthread_t *) arg;
	pthread_cancel(*manager);
	printf("Mainthread will die\n");
}
/*Server

DESCRIPTION: 
Firstly, the socket is configured according to the chosen protocol. Then the threadpool is created alongside the pool_manager thread and the server_administration thread. This threads will run in parallel with the main one. The main one will accept requests from clients and place them in the data structure FIFO. They will be fetched by the threads in the pool as soon as the threads are available

The FIFO queue is protected by a mutex to avoid problems between pool_threads and server. When a request is inserted in FIFO a semaphore is posted so that the pool_threads know there is a pending request. There is also another semaphore that measures the free_pool_threads and controls the maximum size of the FIFO structure.

*/

int main(int argc, char *argv[])
{
/* MUTEX */
	pthread_mutex_init(&mux, NULL);
	pthread_mutex_init(&active_thread_mux,NULL);

/*SIGNALS*/
	/*signal(SIGUSR1,treatment);*/
	int old_cancel_type;
	sigset_t set;
	sigfillset (&set);
    sigprocmask(SIG_BLOCK, &set, NULL);

/* FIFO */
	create_fifo(&front_server, &back_server);
	fifo_count = 0;
	item_server * item;

/* POOL MANAGER */
	pthread_t poolman_t;
	
/* SERVADMIN */
	pthread_t servadmin_t;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_cancel_type);
	pthread_cleanup_push(mainthread_kill,(void*)&poolman_t);
	
/* SOCKETS */
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     pid_t pid;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) {
              perror("ERROR on binding");
              exit(-1);
     }
     listen(sockfd,2); /* 1000 means???*/
     clilen = sizeof(cli_addr);
     
     threadpool(); /* creates threadpool */
     mainthread = pthread_self();
	 pthread_create(&servadmin_t, NULL, servadmin, NULL);
	 pthread_create(&poolman_t, NULL, manager, NULL);

	 	 
	 sem_init(&sem_fifo_used, 0, 0);
	 sem_init(&sem_fifo_free, 0, MAX_FIFO); 
	 
	 while(1){
	 	sem_wait(&sem_fifo_free);
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0){
          perror("ERROR on accept");
        }
        item = (item_server*)malloc(1*sizeof(item_server));
        item->socket = newsockfd;
        item->time = time(NULL);
        pthread_mutex_lock(&mux);
        /* Entering Critical FIFO Region*/
	    queue (&front_server ,&back_server, item);
	    sem_post(&sem_fifo_used);
        fifo_count++;
        /* Exiting Critical FIFO Region*/
        pthread_mutex_unlock(&mux);
     }
     close(sockfd);
     pthread_cleanup_pop(0);
     return 0; 
}
