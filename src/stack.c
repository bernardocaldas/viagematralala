#include <stdio.h>
#include <stdlib.h>
#include <stack.h>


/*
TODO: convergir EmptyStack com DepthStack
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
	Stack * temp= (*top);
	(*top)=(*top)->next;
	return temp->num;
}

int EmptyStack(Stack ** top)
{
	Stack * temp =(*top);
	if(temp == NULL){
		return 0;
	}else{
		return 1;
	}
}

int DepthStack(Stack ** top)
{
	if(EmptyStack(top)==0)
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

}


