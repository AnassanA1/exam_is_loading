#include <unistd.h>
#include <stdio.h>

void set_target(char *str, char *target)
{
    int open = 0;
    int close = 0;

    while (*str)
    {
        if (*str == '(')
            open++;
        if (*str == ')')
        {
            if (open)
                open--;
            else
                close++;
        }
        str++;
    }

    if (open >= close)
        *target = '(';
    else
        *target = ')';
}

int is_balance(char *str)
{
    int open = 0;
    int close = 0;

    while (*str)
    {
        if (*str == '(')
            open++;
        if (*str == ')')
        {
            if (open)
                open--;
            else
                close++;
        }
        str++;
    }

    return (open == 0 && close == 0);
}

void rip(char *str, char target, int index, int len)
{
    if (is_balance(str))
    {
        puts(str);
        return ;
    }

    for (int i = index; i < len; i++)
    {
        if (str[i] == target)
        {
            char tmp = str[i];
            str[i] = ' ';
            rip(str, target, i + 1, len);
            str[i] = tmp;
        }
    }

}


int main(int ac, char **av)
{
    if (ac != 2)
        return (1);
    
    char *str = av[1];
    char target;
    int len = 0;
    while (str[len])
        len++;
    
    set_target(str, &target);
    rip(str, target, 0, len);
    return (0);
}

