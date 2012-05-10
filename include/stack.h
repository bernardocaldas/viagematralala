
#ifndef _STACK_
#define _STACK_

typedef struct StackElement
{
	struct StackElement *next;
	int num;
	int count;
} Stack;

Stack * CreateStack();
void PushStack(Stack **, int);
int PopStack(Stack **);
int DepthStack(Stack ** );
void FreeStack (Stack ** );
void PrintStack(Stack ** );
#endif
