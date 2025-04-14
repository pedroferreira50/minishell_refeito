#include "minishell.h"

int count_args(char **args)
{
    int count;

    count = 0;
    while (args[count])
        count++;
    return (count);
}