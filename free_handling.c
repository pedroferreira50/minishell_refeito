#include "minishell.h"

void free_args(char **args, t_command_data *data)
{
    int i;

	i = 0;
    while (args && args[i])
    {
        free(args[i]);
        i++;
    }
    free(args);
    if (data)
        free_command_data(data);
}