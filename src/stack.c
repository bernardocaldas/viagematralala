#include <stdio.h>
#include <stdlib.h>
#include <stack.h>


/*
TODO: não faltam operações de Free no POP?
*/
Stack * CreateStack()
{
	return NULL;
}

void PushStack(Stack ** top, int num)
{
	Stack * novo = malloc(sizeof(Stack));
	if(*top==NULL){
		novo->count=1;
	}else{
		novo->count=(*top)->count+1;
	}
	novo->num=num;
	novo->next=(*top);
	(*top)=novo;
}

int PopStack(Stack ** top)
{
	int num;
	Stack * temp= (*top);
	(*top)=(*top)->next;
	num = temp->num;
	free(temp);
	return num;
}


int DepthStack(Stack ** top)
{
	if(*top==NULL)
		return 0;
	else
	return (*top)->count;	
}

void FreeStack (Stack ** top){
	Stack * aux;
	aux = (*top);
	while(aux!=NULL){
		free(aux);
		aux = aux->next;
	}
	(*top) = NULL;

}


