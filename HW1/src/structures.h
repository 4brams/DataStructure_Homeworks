#ifndef MAX_STACK_LENGTH
#define MAX_STACK_LENGTH 255
#endif

#ifndef MAX_EXPRESSION_SIZE
#define MAX_EXPRESSION_SIZE 255
#endif

#include <stdint.h>

typedef struct 
{   
    uint8_t length; //0~255
    double items[MAX_STACK_LENGTH];//idx 0~254
}stack;

bool isEmpty(const stack *s)
{
    if(s->length == 0)
    {
        return true;
    }
    
    return false;
}
bool isFull(const stack *s)
{
    if(s->length == MAX_STACK_LENGTH)
    {
        return true;
    }
    
    return false;
}
void init(stack * const s)
{

    s->length = 0;
    return;
}
void push(stack * const s, const double val, bool * const isError)
{
    //printf("%f\n", val);
    if(isFull(s))
    {
        printf("Error: Stack overflow.\n");
        *isError = true;
        return;
    }

    s->items[s->length ++] = val;
    return;
}
double top(const stack * s)
{
    if(isEmpty(s))
    {
        return 0;
    }
    
    return s->items[s->length - 1];
}
double pop(stack * const s)
{
    if(isEmpty(s))
    {
        return 0;
    }
    
    return s->items[-- s->length];
}

void printStack(const stack * s)
{
    if(isEmpty(s))
    {
        return;
    }

    for(int i = s->length - 1; i >= 0; i--)
    {
        printf("idx: %d, val: %.3f\n", i, s->items[i]);
    }

    return;
}