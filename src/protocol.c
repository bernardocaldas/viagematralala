#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 8

void convert_send(int n, char data[]){
	int bendian;
	char be32[9];
	int i;
	
	bendian = htonl(n);
	sprintf(be32, "%X\n", bendian);
	for(i=0; i<SIZE; i++){
		data[i]=be32[i];
	}
}

int convert_recv (char data[]){
	char data_temp[9];
	int bendian;
	int i;
	
	for(i=0; i<SIZE; i++){
		data_temp[i]=data[i];
	}
	data_temp[8]='\0';
	sscanf(data_temp, "%X", &bendian);
	return ntohl(bendian);

}

