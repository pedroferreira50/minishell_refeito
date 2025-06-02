#include "minishell.h"
#include <sys/stat.h>

char	*remove_quotes(char *str)
{
	char	*new_str;
	int		i;
	int		j;
	char	quote_char;

	if (!str)
		return (NULL);
	new_str = malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	quote_char = '\0';
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"') && quote_char == '\0')
		{
			quote_char = str[i];
			i++;
			continue ;
		}
		if (str[i] == quote_char && quote_char != '\0')
		{
			quote_char = '\0';
			i++;
			continue ;
		}
		new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}

char	*expand_tilde(char *token, t_shell *shell)
{
	char	*home;
	char	*result;
	size_t	home_len;
	size_t	token_len;

	if (!token || token[0] != '~')
		return (ft_strdup(token));
	if (token[1] != '\0' && token[1] != '/')
		return (ft_strdup(token));
	home = get_var_value_helper("HOME", shell->vars, shell->envp);
	if (!home || !*home)
		return (ft_strdup(token));
	home_len = ft_strlen(home);
	token_len = ft_strlen(token);
	result = malloc(home_len + token_len);
	if (!result)
		return (ft_strdup(token));
	ft_strcpy(result, home);
	if (token[1] == '/')
		ft_strcat(result, &token[1]);
	return (result);
}

static int	has_mixed_quotes(const char *str)
{
	int		has_single;
	int		has_double;
	int		has_consecutive_quotes;
	int		has_unquoted_content;
	int		in_quotes;
	char	quote_char;
	int		i;

	has_single = 0;
	has_double = 0;
	has_consecutive_quotes = 0;
	has_unquoted_content = 0;
	in_quotes = 0;
	quote_char = '\0';
	i = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"') && !in_quotes)
		{
			if (str[i] == '"' && i > 0 && str[i - 1] == '$')
			{
				i++;
				while (str[i] && str[i] != '"')
					i++;
				if (str[i] == '"')
					i++;
				continue ;
			}
			quote_char = str[i];
			in_quotes = 1;
			if (str[i] == '\'')
				has_single = 1;
			else
				has_double = 1;
			if (i > 0 && str[i - 1] == str[i])
				has_consecutive_quotes = 1;
		}
		else if (str[i] == quote_char && in_quotes)
		{
			in_quotes = 0;
			quote_char = '\0';
			if (str[i + 1] == str[i])
				has_consecutive_quotes = 1;
		}
		else if (!in_quotes && str[i] != '\'' && str[i] != '"')
		{
			if (!(str[i] == '$' && str[i + 1] == '"'))
				has_unquoted_content = 1;
		}
		if ((has_single && has_double) || has_consecutive_quotes
			|| ((has_single || has_double) && has_unquoted_content))
			return (1);
		i++;
	}
	return (0);
}

char	**expand_tokens(char **tokens, char *quote_types, t_shell *shell)
{
	char		**expanded;
	t_indices	idx;
	char		*tmp;

	idx.i = 0;
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
			if (has_mixed_quotes(tokens[idx.i]))
			{
				expanded[idx.i] = expand_mixed_quotes(tokens[idx.i], shell);
				if (!expanded[idx.i])
					expanded[idx.i] = ft_strdup("");
			}
			else
			{
				if (quote_types[idx.i] != '\'')
				{
					expanded[idx.i] = expand_variables(tokens[idx.i],
						quote_types[idx.i], shell);
				}
				else
					expanded[idx.i] = ft_strdup(tokens[idx.i]);
				if (!expanded[idx.i])
					expanded[idx.i] = ft_strdup(tokens[idx.i]);
				tmp = expanded[idx.i];
				expanded[idx.i] = remove_quotes(tmp);
				free(tmp);
				if (!expanded[idx.i])
					expanded[idx.i] = ft_strdup("");
				if (quote_types[idx.i] != '\'')
				{
					tmp = expanded[idx.i];
					expanded[idx.i] = expand_tilde(tmp, shell);
					free(tmp);
				}
			}
		}
		idx.i++;
	}
	expanded[idx.i] = NULL;
	return (expanded);
}

int	validate_command(char **args, t_shell *shell)
{
	struct stat	st;

	if (!args || !args[0])
	{
		shell->exit_status = 0;
		return (0);
	}
	if (args[0] && !*args[0])
	{
		shell->exit_status = 0;
		return (0);
	}
	if (check_builtin(args[0]))
		return (1);
	if (strchr(args[0], '/') != NULL)
	{
		if (stat(args[0], &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(args[0], 2);
				ft_putstr_fd(": Is a directory\n", 2);
				shell->exit_status = 126;
				return (0);
			}
			if (S_ISREG(st.st_mode) && access(args[0], X_OK) != 0)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(args[0], 2);
				ft_putstr_fd(": Permission denied\n", 2);
				shell->exit_status = 126;
				return (0);
			}
		}
		else
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(args[0], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
			shell->exit_status = 127;
			return (0);
		}
	}
	return (1);
}

void	build_command_data(char **args, int argc, t_command_data *data,
	t_shell *shell)
{
	if (data->arguments == NULL)
	{
		data->arguments = malloc(sizeof(char **) * 2);
		if (!data->arguments)
			return ;
		data->arguments[0] = args;
		data->arguments[1] = NULL;
		data->num_commands = 1;
	}
	parse_input(args, argc, data, shell);
}

static void	execute_and_free(t_command_data *data, t_shell *shell)
{
	if (data->arguments != NULL)
	{
		execute_commands(data, shell);
		free(data->arguments);
	}
}

void	expand_and_validate(char **tokens, char *quote_types, t_shell *shell)
{
	char			**expanded;
	t_command_data	data;

	if (!tokens || !tokens[0])
	{
		shell->exit_status = 0;
		return ;
	}
	expanded = expand_tokens(tokens, quote_types, shell);
	if (!expanded)
	{
		shell->exit_status = 1;
		return ;
	}
	if (!validate_command(expanded, shell))
	{
		free_args(expanded, NULL);
		return ;
	}
	build_command_data(expanded, count_args(expanded), &data, shell);
	execute_and_free(&data, shell);
}
