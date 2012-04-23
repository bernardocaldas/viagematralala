int fifo_cnt; /*variavel global*/

typedef struct s_node{
	int socket;
	struct s_node * next;
}fifo_node;

void create_fifo(fifo_node ** front, fifo_node ** back);
fifo_node * dequeue(fifo_node ** front, fifo_node ** back);
void queue (fifo_node ** front, fifo_node ** back, int socket);

