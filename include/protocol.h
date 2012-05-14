/*
Protocol.h

Programação de Sistemas
2011/2012

Authors:
67557 ~ Bernardo Caldas
67636 ~ João Silva
 
*/

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include<arpa/inet.h>
#define LEN 2047 /* fgets reading length*/

typedef struct s_package {
	char data[8];
	char op;
}package;

int convert_recv (char data[]);
void convert_send(int n, char data[]);

#endif
