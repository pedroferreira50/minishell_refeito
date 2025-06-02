#include "minishell.h"

void	print_args(char **args)
{
	int	i;

	if (args == NULL)
	{
		return ;
	}
	i = 0;
	while (args[i] != NULL)
	{
		i++;
	}
}
static void	count_commands(char **args, int count, t_command_data *data,
	int *arg_counts, t_shell *shell)
{
	t_indices	idx;
	int			command_index;

	idx.i = 0;
	idx.j = 0;
	command_index = 0;
	data->num_pipes = 0;
	if (args == NULL)
	{
		shell->exit_status = 2;
		return ;
	}
	while (idx.i < (size_t)count)
	{
		if (args[idx.i] == NULL)
		{
			shell->exit_status = 2;
			return ;
		}
		if (is_operator(args[idx.i]))
		{
			if (ft_strcmp(args[idx.i], "|") == 0)
			{
				if (idx.j == 0)
				{
					shell->exit_status = 2;
					ft_putstr_fd("minishell: syntax error: no command before pipe\n", 2);
					return ;
				}
				if (idx.i + 1 >= (size_t)count || args[idx.i + 1] == NULL)
				{
					shell->exit_status = 2;
					ft_putstr_fd("minishell: syntax error near token\n", 2);
					return ;
				}
				handle_pipe(data, &command_index, shell);
				idx.j = 0;
				idx.i++;
			}
			else if (ft_strcmp(args[idx.i], "<<") == 0)
			{
				if (idx.i + 1 >= (size_t)count || args[idx.i + 1] == NULL)
				{
					shell->exit_status = 2;
					ft_putstr_fd("minishell: syntax error near token\n", 2);
					return ;
				}
				idx.i += 2;  // Skip heredoc operator and delimiter during counting
			}
			else
			{
				if (idx.i + 1 >= (size_t)count || args[idx.i + 1] == NULL)
				{
					shell->exit_status = 2;
					ft_putstr_fd("minishell: syntax error near token\n", 2);
					return ;
				}
				shell->is_counting = 1;
				handle_redirect(args, data, &idx, shell, command_index);
			}
		}
		else
		{
			arg_counts[command_index]++;
			idx.j++;
			idx.i++;
		}
	}
	data->num_commands = command_index + 1;
	if (data->num_commands <= data->num_pipes)
	{
		shell->exit_status = 2;
		ft_putstr_fd("minishell: invalid pipe sequence\n", 2);
		data->num_commands = 0;
	}
}


static void	alloc_commands(t_command_data *data, t_shell *shell)
{
	int i;

	data->commands = malloc(data->num_commands * sizeof(char *));
	data->arguments = malloc(data->num_commands * sizeof(char **));
	data->input_files = malloc(data->num_commands * sizeof(char *));
	data->out_redirs = malloc(data->num_commands * sizeof(t_redirection *));
	data->num_out_redirs = malloc(data->num_commands * sizeof(int));
	if (data->commands == NULL || data->arguments == NULL ||
		data->input_files == NULL || data->out_redirs == NULL ||
		data->num_out_redirs == NULL)
	{
		shell->exit_status = 1;
		free_command_data(data);
		return;
	}
	ft_memset(data->commands, 0, data->num_commands * sizeof(char *));
	ft_memset(data->arguments, 0, data->num_commands * sizeof(char **));
	ft_memset(data->input_files, 0, data->num_commands * sizeof(char *));
	ft_memset(data->out_redirs, 0, data->num_commands * sizeof(t_redirection *));
	ft_memset(data->num_out_redirs, 0, data->num_commands * sizeof(int));
	i = 0;
	while (i < data->num_commands)
	{
		data->num_out_redirs[i] = 0;
		i++;
	}
}
static void	populate_command(char **args, int *arg_counts, t_command_data *data, t_parse_state *state)
{
	if (state->idx.j == 0)
	{
		data->commands[state->command_index] = ft_strdup(args[state->idx.i]);
		data->arguments[state->command_index] = malloc((arg_counts[state->command_index] + 1) * sizeof(char *));
		if (data->commands[state->command_index] == NULL || data->arguments[state->command_index] == NULL)
			return;
	}
}


static void	populate_argument(char **args, t_command_data *data, t_parse_state *state)
{
	if (state->idx.j == 0 && args[state->idx.i] && !*args[state->idx.i])
	{
		while (args[state->idx.i] && !*args[state->idx.i])
			state->idx.i++;
		
		if (!args[state->idx.i])
			return;
		
		if (data->commands[state->command_index])
			free(data->commands[state->command_index]);
		data->commands[state->command_index] = ft_strdup(args[state->idx.i]);
	}
	
	data->arguments[state->command_index][state->idx.j] = ft_strdup(args[state->idx.i]);
	if (data->arguments[state->command_index][state->idx.j] == NULL)
		return ;
	state->idx.j++;
	state->idx.i++;
}

static void	handle_operator_wrapper(char **args, t_command_data *data,
	t_parse_state *state, t_shell *shell)
{
	if (args[state->idx.i] == NULL)
		return ;
	if (ft_strcmp(args[state->idx.i], "|") == 0)
	{
		state->command_index++;
		state->idx.j = 0;
		state->idx.i++;
		return ;
	}
	if (ft_strcmp(args[state->idx.i], "<<") == 0)
		handle_heredoc(args, data, &state->idx, shell);
	else
	{
		shell->is_counting = 0;
		handle_redirect(args, data, &state->idx, shell, state->command_index);
	}
}
static void	populate_commands(char **args, int *arg_counts,
	t_command_data *data, t_shell *shell)
{
	t_parse_state	state;
	size_t			arg_count;

	state.command_index = 0;
	state.idx.i = 0;
	state.idx.j = 0;
	arg_count = 0;
	if (args == NULL)
	{
		shell->exit_status = 2;
		return ;
	}
	arg_count = count_args(args);
	while (state.idx.i < arg_count && args[state.idx.i] != NULL)
	{
		if (is_operator(args[state.idx.i]))
			handle_operator_wrapper(args, data, &state, shell);
		else
		{
			populate_command(args, arg_counts, data, &state);
			populate_argument(args, data, &state);
		}
	}
}
void	parse_input(char **args, int count, t_command_data *data, t_shell *shell)
{
	int	*arg_counts;
	int	i;

	arg_counts = NULL;
	ft_memset(data, 0, sizeof(t_command_data));
	arg_counts = malloc(count * sizeof(int));
	if (arg_counts == NULL)
	{
		shell->exit_status = 1;
		return ;
	}
	ft_memset(arg_counts, 0, sizeof(int) * count);
	count_commands(args, count, data, arg_counts, shell);
	if (data->num_commands == 0)
	{
		free_command_data(data);
		free(arg_counts);
		return ;
	}
	alloc_commands(data, shell);
	if (data->commands == NULL || data->arguments == NULL)
	{
		free(arg_counts);
		return ;
	}
	populate_commands(args, arg_counts, data, shell);
	i = 0;
	while (i < data->num_commands)
	{
		if (data->arguments[i] != NULL)
			data->arguments[i][arg_counts[i]] = NULL;
		i++;
	}
	free(arg_counts);
}
