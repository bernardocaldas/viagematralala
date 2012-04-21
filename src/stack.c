#include <stdio.h>
#include <stdlib.h>
#include <stack.h>

Stack * CreateStack()
{
	return NULL;
}

void PushStack(Stack ** inicio, int num)
{
	Stack * novo = malloc(sizeof(Stack));
	if(*inicio==NULL){
		novo->count=1;
	}else{
		novo->count=(*inicio)->count+1;
	}
	novo->num=num;
	novo->next=(*inicio);
	(*inicio)=novo;
}

int PopStack(Stack ** inicio)
{
	Stack * temp= (*inicio);
	(*inicio)=(*inicio)->next;
	return temp->num;
}

int OneOp(Stack ** inicio)
{
	if((*inicio)->count==1){
		return 0;
	}
	else{
		return 1;
	}
}

int EmptyStack(Stack ** inicio)
{
	Stack * temp =(*inicio);
	if(temp == NULL){
		return 0;
	}else{
		return 1;
	}
}
