#include "minishell.h"

char **copy_envp(char **envp)
{
    int i;
    char **new_envp;

    i = 0;
    while (envp[i])
        i++;
    new_envp = malloc((i + 1) * sizeof(char *));
    if (!new_envp)
        return (NULL);
    i = 0;
    while (envp[i])
    {
        new_envp[i] = ft_strdup(envp[i]);
        i++;
    }
    new_envp[i] = NULL;
    return (new_envp);
}