#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stack.h>
#include <string.h>


typedef struct s_targ {

	int socket;

}targ;

void * yasc (void * arg);
