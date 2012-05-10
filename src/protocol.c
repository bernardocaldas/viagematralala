
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>

char * convert_send (int a){
	char * data_temp; 
	data_temp = calloc(9,sizeof(char));
	
	sprintf(data_temp, "%X\n", htobe32(a));
	printf("%d\n",be32toh(htobe32(a)));
	printf("%s\n", data_temp);
	return data_temp;
}

int convert_recv (char * b){
	char * data, * data2;
	int num;
	data = calloc(9,sizeof(char));
	data2 = calloc(9,sizeof(char));
	
	data=b; 
	printf("%s\n", data);
	data[8]='\0';
	num = strtod(data,NULL);
	printf("%d\n", be32toh(num));
	return be32toh(num);
}
