#include <stdint.h>
#include <stddef.h>

typedef double (*Operation)(double, double);

double add(const double a, const  double b)
{
    return a + b;
}

double subtract(const double a, const double b)
{
    return a - b;
}

double multiply(const double a, const double b)
{
    return a * b;
}

double divide(const double a, const double b)
{
    return a / b;
}

double power(const double a, const double b)
{
    double temp = 1;
    for(int i = 0; i < b; i++)
    {
        temp *= a;
    }
    return temp;
}

Operation getOperation(const char opr)
{
    switch (opr)
    {
    case '+':
        return add;
    case '-':
        return subtract;
    case '*':
        return multiply;
    case '/':
        return divide;
    case '^':
        return power;
    default:
        return NULL;
    }
}

double priority(const char opr)
{
    switch (opr){
        case '^':
            return 30;
        case '*':
            return 20;
        case '/':
            return 20;
        case '+':
            return 10;
        case '-':
            return 10;
        case '(':
            return 0;
        default:
            return -1;
    }
}