#include "minishell.h"
#include <sys/stat.h>

char *remove_quotes(char *str)
{
    char *new_str;
    int i = 0, j = 0;
    char quote_char = '\0';

    if (!str)
        return (NULL);

    new_str = malloc(ft_strlen(str) + 1);
    if (!new_str)
		return (NULL);
    while (str[i])
    {
        if ((str[i] == '\'' || str[i] == '"') && quote_char == '\0')
        {
            quote_char = str[i];
            i++;
            continue;
        }
        if (str[i] == quote_char && quote_char != '\0')
        {
            quote_char = '\0';
            i++;
            continue;
        }
        new_str[j++] = str[i++];
    }

    new_str[j] = '\0';
    return (new_str);
}



char **expand_tokens(char **tokens, char *quote_types, t_shell *shell)
{
	char **expanded;
	t_indices idx;
	char *tmp;

	idx.i = 0;
	if (!tokens || !tokens[0])
		return (NULL);
	expanded = malloc(sizeof(char *) * (count_args(tokens) + 1));
	if (!expanded)
		return (NULL);
	while (tokens[idx.i])
	{
		if (idx.i > 0 && ft_strcmp(tokens[idx.i - 1], "<<") == 0)
		{
			expanded[idx.i] = ft_strdup(tokens[idx.i]);
		}
		else
		{
			if (quote_types[idx.i] != '\'')
				expanded[idx.i] = expand_variables(tokens[idx.i], quote_types[idx.i], shell);
			else
				expanded[idx.i] = ft_strdup(tokens[idx.i]);
			if (!expanded[idx.i])
				expanded[idx.i] = ft_strdup(tokens[idx.i]);
			tmp = expanded[idx.i];
			expanded[idx.i] = remove_quotes(tmp);
			free(tmp);
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

void build_command_data(char **args, int argc, t_command_data *data, t_shell *shell)
{
	if (data->arguments == NULL)
	{
		data->arguments = malloc(sizeof(char **) * 2);
		if (!data->arguments)
			return;
		data->arguments[0] = args;
		data->arguments[1] = NULL;
		data->num_commands = 1;
	}
	parse_input(args, argc, data, shell);
}

static void execute_and_free(t_command_data *data, t_shell *shell)
{
	if (data->arguments != NULL)
	{
		execute_commands(data, shell);
		free(data->arguments);
	}
}

void expand_and_validate(char **tokens, char *quote_types, t_shell *shell)
{
	char			**expanded;
	t_command_data	data;

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
	build_command_data(expanded, count_args(expanded), &data, shell);
	execute_and_free(&data, shell);
}
