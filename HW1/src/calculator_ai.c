#define MAX_STACK_LENGTH 255
#define MAX_EXPRESSION_SIZE 512
#define DEBUG false

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
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
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("==================================================\n");
    printf(" Current Time: %s                \n", buffer);
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

static bool calculateOne(stack * const operatorStack, stack * const operandStack)
{
    if(operandStack->length < 2 || operatorStack->length == 0)
    {
        return false;
    }

    const char operator = (char)pop(operatorStack);
    const double right = pop(operandStack);
    const double left = pop(operandStack);

    if(operator == '/' && right == 0)
    {
        printf("Error: Can't be divided by 0.\n");
        return false;
    }

    Operation operation = getOperation(operator);
    if(operation == NULL)
    {
        return false;
    }

    bool isError = false;
    push(operandStack, operation(left, right), &isError);
    return !isError;
}

static bool evaluateExpression(const char *expression, double *result)
{
    stack operandStack;
    stack operatorStack;
    bool isError = false;
    bool expectingOperand = true;
    bool sawToken = false;

    init(&operandStack);
    init(&operatorStack);

    for(size_t i = 0; expression[i] != '\0' && expression[i] != '\n';)
    {
        const char token = expression[i];

        if(isspace((unsigned char)token))
        {
            i++;
            continue;
        }

        if(isdigit((unsigned char)token) || token == '.'
            || (expectingOperand && (token == '+' || token == '-')
                && (isdigit((unsigned char)expression[i + 1]) || expression[i + 1] == '.')))
        {
            char *end;
            const double number = strtod(expression + i, &end);
            if(end == expression + i)
            {
                return false;
            }
            push(&operandStack, number, &isError);
            if(isError)
            {
                return false;
            }
            i = (size_t)(end - expression);
            expectingOperand = false;
            sawToken = true;
            continue;
        }

        if(token == '(')
        {
            if(!expectingOperand)
            {
                return false;
            }
            push(&operatorStack, token, &isError);
            if(isError)
            {
                return false;
            }
            i++;
            sawToken = true;
            continue;
        }

        if(token == ')')
        {
            if(expectingOperand)
            {
                return false;
            }
            while(operatorStack.length > 0 && top(&operatorStack) != '(')
            {
                if(!calculateOne(&operatorStack, &operandStack))
                {
                    return false;
                }
            }
            if(operatorStack.length == 0)
            {
                return false;
            }
            pop(&operatorStack);
            i++;
            expectingOperand = false;
            continue;
        }

        if(type(token) == 2 && !expectingOperand)
        {
            while(operatorStack.length > 0 && top(&operatorStack) != '('
                && (priority((char)top(&operatorStack)) > priority(token)
                    || (priority((char)top(&operatorStack)) == priority(token) && token != '^')))
            {
                if(!calculateOne(&operatorStack, &operandStack))
                {
                    return false;
                }
            }
            push(&operatorStack, token, &isError);
            if(isError)
            {
                return false;
            }
            i++;
            expectingOperand = true;
            sawToken = true;
            continue;
        }

        return false;
    }

    if(!sawToken || expectingOperand)
    {
        return false;
    }

    while(operatorStack.length > 0)
    {
        if(top(&operatorStack) == '(' || !calculateOne(&operatorStack, &operandStack))
        {
            return false;
        }
    }

    if(operandStack.length != 1)
    {
        return false;
    }
    *result = pop(&operandStack);
    return true;
}

void getExpression(struct ev_loop *loop, ev_io *watcher, int revents)
{
    char buffer[MAX_EXPRESSION_SIZE];
    int n = read(watcher->fd, buffer, sizeof(buffer) - 1);
    if (n > 0)
    {
        buffer[n] = '\0';
        if(n >= 4 && memcmp(buffer, "exit", 4) == 0)
        {
            exit(0);
        }

        double result;
        if(evaluateExpression(buffer, &result))
        {
            printf("Result\t\t=%f\n", result);
        }
        else
        {
            printf("Error: Invalid Expression.\n");
        }
    }

    printf("\nExpression\t>");
}

void updateTime(struct ev_loop *loop, ev_timer *watcher, int revents)
{
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