#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stack.h>
#include <string.h>

#define MAXLINHA 20


int main(int argc, char* argv[])
{	
	Stack * stack = CreateStack();
	char entrada[MAXLINHA];
	char ctemp;
	int itemp,opA,opB,opResult;
	char * result;
	char delims[2]={' ',';'};
	while(fgets(entrada,MAXLINHA-1,stdin)!=NULL)
	{
		result=strtok(entrada,delims);
		while(result!=NULL)
		{
			if(sscanf(result,"%d",&itemp)==1)
			{
				printf("Apanhei o numero %d\n",itemp);
				PushStack(&stack, itemp);
			}
			else
			{
				sscanf(result,"%c",&ctemp);
				if(ctemp=='+')
				{
					printf("Apanhei uma soma\n");
					opA=PopStack(&stack);
					opB=PopStack(&stack);
					opResult=opA+opB;
					PushStack(&stack,opResult);
					printf("%d+%d=%d\n",opA,opB,opResult);
				}
			}
			result=strtok(NULL,delims);
		}
	}
	return 0;
}
