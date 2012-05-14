/*
Protocol.c

Programação de Sistemas
2011/2012

Authors:
67557 ~ Bernardo Caldas
67636 ~ João Silva
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <protocol.h>
#define SIZE 8
/* 
FUNCTION: convert_send

DESCRIPTION:
Converts integer 'n' given as an argument to big-endian notation; the big-endian number will then be written in a string as an hexadecimal number. The string will then be converted to a sequence of 8 char  that is stored in the array 'data'.
*/
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
/* 
FUNCTION: convert_recv

DESCRIPTION:
Dual function of convert_send: if provided with a char ASCII array in big-endian and hexadecimal notation it will return the corresponding integer.
*/

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

