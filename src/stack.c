#include <stack.h>

Stack * CreateStack()
{
	return NULL;
}

void PushStack(Stack ** inicio, int num)
{
	Stack * novo = malloc(sizeof(Stack));
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
