#include "minishell.h"
#include <sys/stat.h>

char **expand_tokens(char **tokens, char *quote_types, t_shell *shell)
{
    char **expanded;
    t_indices idx;

	idx.i = 0;
	idx.j = 0;
    if (!tokens || !tokens[0])
        return (NULL);
    expanded = malloc(sizeof(char *) * (count_args(tokens) + 1));
    if (!expanded)
        return (NULL);
    while (tokens[idx.i])
    {
        if (idx.i > 0 && ft_strcmp(tokens[idx.i - 1], "<<") == 0)
            expanded[idx.i] = ft_strdup(tokens[idx.i]);
        else
        {
            expanded[idx.i] = expand_variables(tokens[idx.i], quote_types[idx.i], shell);
            if (!expanded[idx.i])
                expanded[idx.i] = ft_strdup(tokens[idx.i]);
            if (quote_types[idx.i] == '"' || quote_types[idx.i] == '\'')
            {
                char *tmp = expanded[idx.i];
                size_t len = ft_strlen(tmp);
                if (len >= 2 && tmp[0] == quote_types[idx.i] && tmp[len - 1] == quote_types[idx.i])
                {
                    expanded[idx.i] = ft_strndup(tmp + 1, len - 2);
                    free(tmp);
                }
            }
        }
        idx.i++;
    }
    expanded[idx.i] = NULL;
    return (expanded);
}

int validate_command(char **args, t_shell *shell)
{
    struct stat st; //verify if we can use this

    if (!args || !args[0])
    {
        shell->exit_status = 0;
        return (0);
    }
    if (ft_strcmp(args[0], "~") == 0)
    {
        char *home = getenv("HOME");
        free(args[0]);
        args[0] = ft_strdup(home ? home : "");
    }
    if (ft_strcmp(args[0], ".") == 0 || ft_strcmp(args[0], "..") == 0)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(args[0], 2);
        ft_putstr_fd(": command not found\n", 2);
        shell->exit_status = 127;
        return (0);
    }
    if (stat(args[0], &st) == 0 && S_ISDIR(st.st_mode))
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(args[0], 2);
        ft_putstr_fd(": Is a directory\n", 2);
        shell->exit_status = 126;
        return (0);
    }
    return (1);
}

static void build_command_data(char **args, int argc, t_command_data *data)
{
    ft_memset(data, 0, sizeof(t_command_data));
    data->arguments = malloc(sizeof(char **) * 2);
    if (!data->arguments)
        return;
    data->arguments[0] = args;
    data->arguments[1] = NULL;
    data->num_commands = 1;
    parse_input(args, argc, data);
}

static void execute_and_free(t_command_data *data, t_shell *shell)
{
    if (data->arguments)
    {
        execute_commands(data, &shell->envp, &shell->vars);
        free(data->arguments);
    }
}

void expand_and_validate(char **tokens, char *quote_types, t_shell *shell)
{
    char **expanded;
    t_command_data data;

    if (!tokens || !tokens[0])
    {
        shell->exit_status = 0;
        return;
    }
    expanded = expand_tokens(tokens, quote_types, shell);
    if (!expanded)
    {
        shell->exit_status = 1;
        return;
    }
    if (!validate_command(expanded, shell))
    {
        free_args(expanded, NULL);
        return;
    }
    build_command_data(expanded, count_args(expanded), &data);
    execute_and_free(&data, shell);
}
