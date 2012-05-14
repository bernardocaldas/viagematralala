#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_PID 100

int main(){
	pid_t pid;
	int i;
	char path[256];
	
	pid=fork();
	strcpy(path, "./bin/client");
	for(i=0; i<MAX_PID; i++){
		if(pid != 0){
			pid=fork();
		}else{
			if(pid == 0)
				execl(path, "client", "localhost", "2000", "ordens5.txt", NULL);
		}
	}
	for(i=0;i<MAX_PID+1;i++){
		wait();
	}
	
	return 0;
}
