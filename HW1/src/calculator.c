#define MAX_STACK_LENGTH 255
#define MAX_EXPRESSION_SIZE 255
#define DEBUG false

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ev.h>
#include <time.h>
#include <unistd.h>

#include "operations.h"
#include "structures.h"

void printCurrentTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    rawtime += 8 * 60 * 60;
    timeinfo = gmtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("==================================================\n");
    printf("Current Time: %s         \n", buffer);
    printf("==================================================\n");
}

uint8_t type(const char c)
{
    if(((c >= '0') && (c <= '9')) || c == '.')
    {
        return 1;
    }
    else if(c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
    {
        return 2;
    }
    else if(c == '(')
    {
        return 3;
    }
    else if(c == ')')
    {
        return 4;
    }
    else if(c == '\n' || c == '\0')
    {
        return 5;
    }
    else
    {
        return 0;
    }
}

void calculateOne(stack * const operatorStack, stack * const operandStack, bool * const isError)
{
    if(*isError)
    {
        return;
    }

    //printf("oprl: %d, opdl:%d, topopr:%cx\n", operandStack->length, operatorStack->length, (int)top(operatorStack));

    if(operandStack->length >= 2 && operatorStack->length >= 1)
    {
        char operator = pop(operatorStack);
        double first = pop(operandStack);
        double second = pop(operandStack);

        if(first == 0 && operator == '/')
        {
            printf("Error: Can't be divided by 0.\n");
            *isError = true;
            return;
        }

        Operation operation = getOperation(operator);
        push(operandStack, operation(second, first), isError);
    }
    else if(operandStack->length == 1 && operatorStack->length == 1 && top(operatorStack) == '-')
    {
        char operator = pop(operatorStack);
        double first = pop(operandStack);
        double second = 0;
        Operation operation = getOperation(operator);
        push(operandStack, operation(second, first), isError);
    }
    else
    {
        printf("Error: Invalid Expression.\n");
        *isError = true;
        return;
    }
}

void pushOperators(stack * const operatorStack, stack * const operandStack, const char operator, bool * const isError)
{
    if(*isError)
    {
        return;
    }

    while(priority(operator) <= priority((int)top(operatorStack)))
    {
        calculateOne(operatorStack, operandStack, isError);
    }
    push(operatorStack, operator, isError);
}

uint8_t handleOperator(stack * const operatorStack, stack * const operandStack, const char * expression, const uint8_t startFrom, bool * const isError)
{   
    if(*isError)
    {
        return startFrom;
    }

    const bool isAddOrSub = (expression[startFrom] == '+' || expression[startFrom] == '-');
    uint8_t PositiveOrNegative = 1;
    uint8_t countMulOrDivOrPow = 0;

    for(uint8_t i = startFrom; i < MAX_EXPRESSION_SIZE; i++)
    {
        if(expression[i] == ' ')
        {
            continue;
        }

        if(type(expression[i]) == 0){
            printf("Error: Unknown token '%c'.\n", expression[i]);
            *isError = true;
            return i;
        }

        if(type(expression[i]) != 2)
        {   
            if(isAddOrSub)
            {
                if(PositiveOrNegative == 1)
                {
                    pushOperators(operatorStack, operandStack, '+', isError);
                }
                else
                {
                    pushOperators(operatorStack, operandStack, '-', isError);
                }
            }
            else
            {
                pushOperators(operatorStack, operandStack, expression[startFrom], isError);
            }
            return i;
        }

        if(isAddOrSub)
        {
            if(!(expression[i] == '+' || expression[i] == '-'))
            {
                printf("Error: Invalid expression.\n");
                *isError = true;
                return i;
            }

            if(expression[i] == '-')
            {
                PositiveOrNegative *= -1;
            }
        }
        else
        {
            countMulOrDivOrPow++;

            if(countMulOrDivOrPow > 1){
                printf("Error: Invalid expression.\n");
                *isError = true;
                return i;
            }
        }
    }
    return 0;
}

uint8_t handleOperand(stack * const operandStack, const char * expression, const uint8_t startFrom, bool * const isError)
{   
    if(*isError)
    {
        return startFrom;
    }

    double number = 0;
    uint8_t afterPoint = 0;
    bool isAfterPoint = false;

    for(uint8_t i = startFrom; i < MAX_EXPRESSION_SIZE; i++)
    {
        if(expression[i] == ' ')
        {
            continue;
        }

        if(type(expression[i]) == 0){
            printf("Error: Unknown token '%c'.\n", expression[i]);
            *isError = true;
            return i;
        }

        if(type(expression[i]) != 1)
        {
            push(operandStack, number * power(0.1, afterPoint), isError);
            //printf("opn:%f, stack:%f, i:%d\n", number * power(0.1, afterPoint), top(operandStack), i);
            return i;
        }

        if(expression[i] == '.')
        {
            isAfterPoint = true;
            continue;
        }

        number = number * 10 + (expression[i] - '0');

        if(isAfterPoint)
        {
            afterPoint++;
        }
    }

    *isError = true;
    return 0;
}

uint8_t handleExpression(const char * expression, const uint8_t startFrom, double * const result, bool * const isError)
{
    stack operandStack, operatorStack;

    uint8_t mode = 0; //1 for numbers, 2 for operators, 3 for expression, 4 for end expression, 5 for end
    
    init(&operandStack);
    init(&operatorStack);

    for(uint8_t i = startFrom; i < MAX_EXPRESSION_SIZE;)
    {   
        if(*isError)
        {   
            return i;
        }

        mode = type(expression[i]);
        //printf("i:%d, char:%c, isErr:%d, mode:%d\n", i, expression[i], *isError, mode);

        if(expression[i] == ' ')
        {
            i++;
            continue;
        }

        switch (mode)
        {
        case 1:
            i = handleOperand(&operandStack, expression, i, isError);
            break;
        case 2:
            i = handleOperator(&operatorStack, &operandStack, expression, i, isError);
            break;
        case 3:
            double subExpResult = 0;
            i = handleExpression(expression, i+1, &subExpResult, isError);
            i++;
            push(&operandStack, subExpResult, isError);
            break;
        case 4:
        case 5:
            while(!(operatorStack.length == 0 && operandStack.length == 1) && !*isError)
            {
                calculateOne(&operatorStack, &operandStack, isError);
            }
            *result = pop(&operandStack);
            return i;
            break;
        case 0:
        default:
            printf("Error: Unknown token '%c'.\n", expression[i]);
            *isError = true;
            return i;
        }
    }
    return 0;
}

void getExpression(struct ev_loop *loop, ev_io *watcher, int revents)
{
    (void)loop;
    (void)revents;
    char buffer[MAX_EXPRESSION_SIZE];
    bool isError = false;
    int n = read(watcher->fd, buffer, sizeof(buffer) - 1);
    if (n > 0)
    {
        buffer[n] = '\0';
        //printf("Buffer>%s", buffer);
        if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
        {
            exit(0);
        }

        double result = 0;
        uint8_t i = handleExpression(buffer, 0, &result, &isError);

        if(!isError)
        {
        printf("Result\t\t=%f\n", result);
        }
        else
        {
            printf("%s", buffer);
            for(uint8_t j = 0; j < (i); j++)
            {
                printf(" ");
            }
            printf("^\n");
        }
    }

    printf("\nExpression\t>");
}

void updateTime(struct ev_loop *loop, ev_timer *watcher, int revents)
{
    (void)loop;
    (void)watcher;
    (void)revents;
    printf("\x1b[s\x1b[H");
    printCurrentTime();
    printf("\x1b[u");
    fflush(stdout);
}

int main(void)
{
    struct ev_loop *loop = EV_DEFAULT;

    //time
    printf("\x1b[2J");
    fflush(stdout);
    ev_timer timer_watcher;
    ev_timer_init(&timer_watcher, updateTime, 1.0, 1.0);
    ev_timer_start(loop, &timer_watcher);

    //expression
    ev_io io_watcher;
    int fd = STDIN_FILENO; 
    printf("Expression\t>");
    ev_io_init(&io_watcher, getExpression, fd, EV_READ);
    ev_io_start(loop, &io_watcher);

    ev_run(loop, 0);
    
    return 0;
}