#include "minishell.h"

void	handle_pipe(t_command_data *data, int *command_index, t_shell *shell)
{
	data->num_pipes++;
	(*command_index)++;
	shell->exit_status = 0;
}

void	handle_redirect(char **args, t_command_data *data, t_indices *indices,
	t_shell *shell, int cmd_index)
{
	const char		*token;
	int				fd;
	int				flags;
	t_redirection	*new_redirs;

	token = args[indices->i];
	if (token == NULL || args[indices->i + 1] == NULL)
	{
		print_error_simple("syntax error near unexpected token `newline'", 2, shell);
		indices->i++;
		return ;
	}
	if (is_operator(args[indices->i + 1]))
	{
		ft_putstr_fd("bash: syntax error near unexpected token `", 2);
		ft_putstr_fd(args[indices->i + 1], 2);
		ft_putstr_fd("'\n", 2);
		shell->exit_status = 2;
		indices->i++;
		return ;
	}
	if (ft_strcmp(token, "<") == 0)
	{
		if (!shell->is_counting)
		{
			if (shell->exit_status == 0)
			{
				fd = open(args[indices->i + 1], O_RDONLY);
				if (fd < 0)
				{
					ft_putstr_fd("minishell: ", 2);
					perror(args[indices->i + 1]);
					shell->exit_status = 1;
				}
				else
				{
					close(fd);
				}
			}
			if (data->input_files)
			{
				free(data->input_files[cmd_index]);
				data->input_files[cmd_index] = ft_strdup(args[indices->i + 1]);
				if (data->input_files[cmd_index] == NULL)
					shell->exit_status = 1;
			}
		}
		indices->i += 2;
	}
	else if (ft_strcmp(token, ">") == 0 || ft_strcmp(token, ">>") == 0)
	{
		if (!shell->is_counting && data->out_redirs && data->num_out_redirs)
		{
			if (shell->exit_status == 0)
			{
				flags = O_WRONLY | O_CREAT;
				if (ft_strcmp(token, ">>") == 0)
					flags |= O_APPEND;
				else
					flags |= O_TRUNC;
				fd = open(args[indices->i + 1], flags, 0666);
				if (fd < 0)
				{
					ft_putstr_fd("minishell: ", 2);
					perror(args[indices->i + 1]);
					shell->exit_status = 1;
				}
				else
				{
					close(fd);
				}
			}
			new_redirs = realloc(data->out_redirs[cmd_index],
				(data->num_out_redirs[cmd_index] + 1) * sizeof(t_redirection));
			if (new_redirs == NULL)
			{
				shell->exit_status = 1;
				indices->i += 2;
				return ;
			}
			data->out_redirs[cmd_index] = new_redirs;
			data->out_redirs[cmd_index][data->num_out_redirs[cmd_index]].file
				= ft_strdup(args[indices->i + 1]);
			if (ft_strcmp(token, ">>") == 0)
				data->out_redirs[cmd_index][data->num_out_redirs[cmd_index]].append = 1;
			else
				data->out_redirs[cmd_index][data->num_out_redirs[cmd_index]].append = 0;
			if (data->out_redirs[cmd_index][data->num_out_redirs[cmd_index]].file
				== NULL)
				shell->exit_status = 1;
			data->num_out_redirs[cmd_index]++;
		}
		indices->i += 2;
	}
	else
	{
		print_error_simple("syntax error near redirect", 2, shell);
		indices->i++;
	}
}


void	handle_heredoc(char **args, t_command_data *data, t_indices *indices,
	t_shell *shell)
{
	size_t	len;
	char	*delim;
	char	*clean_delim;
	size_t	j;
	size_t	i;

	if (args[indices->i + 1] != NULL)
	{
		if (data->heredoc_delim)
		{
			free(data->heredoc_delim);
			data->heredoc_delim = NULL;
		}
		delim = args[indices->i + 1];
		len = ft_strlen(delim);
		if (len >= 2 && (delim[0] == '"' || delim[0] == '\'')
			&& delim[len - 1] == delim[0])
		{
			data->heredoc_delim = ft_strndup(delim + 1, len - 2);
			data->heredoc_quoted = 1;
		}
		else if (ft_strchr(delim, '\'') || ft_strchr(delim, '"'))
		{
			clean_delim = malloc(len + 1);
			j = 0;
			i = 0;
			if (clean_delim)
			{
				while (i < len)
				{
					if (delim[i] != '\'' && delim[i] != '"')
					{
						clean_delim[j] = delim[i];
						j++;
					}
					i++;
				}
				clean_delim[j] = '\0';
				data->heredoc_delim = clean_delim;
				data->heredoc_quoted = 1;
			}
			else
			{
				data->heredoc_delim = ft_strdup(delim);
				data->heredoc_quoted = 0;
			}
		}
		else
		{
			data->heredoc_delim = ft_strdup(delim);
			data->heredoc_quoted = 0;
		}
		if (data->heredoc_delim == NULL)
			shell->exit_status = 1;
		indices->i += 2;
	}
	else
	{
		print_error_simple("syntax error: no delimiter after '<<'", 2, shell);
		indices->i++;
	}
}
