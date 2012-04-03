#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stack.h>


int main(int argc, char* argv[])
{
	int a,b,c;
	Stack * inicio = CreateStack();	
	scanf("%d",&a);
	scanf("%d",&b);
	scanf("%d",&c);
	PushStack(&inicio,a);
	PushStack(&inicio,b);
	PushStack(&inicio,c);
	printf("Pela ordem inversa:%d",PopStack(&inicio));
	printf("Pela ordem inversa:%d",PopStack(&inicio));
	printf("Pela ordem inversa:%d",PopStack(&inicio));
	return 0;
}
