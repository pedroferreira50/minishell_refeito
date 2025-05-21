/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_commands.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:49:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/21 17:53:05 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	finalize_execution(t_exec_state *state, pid_t *pids,
	t_command_data *data, t_shell *shell)
{
	int save_exit;

	save_exit = shell->exit_status;
	wait_commands(pids, data, shell);
	if(!get_shell()->is_save_to_execute)
		shell->exit_status = save_exit;
	get_shell()->is_save_to_execute = true;
	free_command_data(data);
	if (state->prev_pipe_read != -1)
	{
		close(state->prev_pipe_read);
		state->prev_pipe_read = -1;
	}
}

pid_t	*init_execution(t_command_data *data, t_exec_state *state,
	t_shell *shell)
{
	pid_t	*pids;

	pids = NULL;
	if (data == NULL || data->commands == NULL || data->num_commands == 0)
	{
		shell->exit_status = 2;
		return (NULL);
	}
	pids = malloc(sizeof(pid_t) * data->num_commands);
	if (pids == NULL)
	{
		shell->exit_status = 1;
		return (NULL);
	}
	ft_memset(pids, 0, sizeof(pid_t) * data->num_commands);
	state->prev_pipe_read = -1;
	state->heredoc_fd = -1;
	state->i = 0;
	return (pids);
}

void	execute_parent(t_command_data *data, t_exec_state *state,
	t_shell *shell)
{
	if (ft_strcmp(data->commands[state->i], "cd") == 0)
	{
		ft_cd(data->arguments[state->i], &state->i, shell);
		get_shell()->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "export") == 0)
	{
		shell->exit_status = ft_export(data->arguments[state->i], shell);
		get_shell()->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "unset") == 0)
	{
		shell->exit_status = ft_unset(data->arguments[state->i], \
				&shell->vars, &shell->envp);
		get_shell()->is_save_to_execute = false;
	}
	else if (ft_strcmp(data->commands[state->i], "exit") == 0)
	{
		ft_exit(data->arguments[state->i], shell);
		get_shell()->is_save_to_execute = false;
	}
}

int	parent_builtin(t_command_data *data, t_exec_state *state, t_shell *shell)
{
	char *cmd;

	get_shell()->is_save_to_execute = true;
	if (!data || !data->commands[state->i])
		return (0);
	cmd = data->commands[state->i];
	if (data->num_commands == 1 && check_builtin(cmd) && !data->input_file && data->num_out_redirs == 0 && state->heredoc_fd == -1)
	{
		execute_parent(data, state, shell);
		return (1);
	}
	return (0);
}

// void print_arguments(char ***arguments)
// {
//     int i = 0;
//     int j;

//     if (!arguments)
//     {
//         printf("arguments is NULL\n");
//         return;
//     }
//     while (arguments[i])
//     {
//         printf("Command [%d]:\n", i);
//         j = 0;
//         while (arguments[i][j])
//         {
//             printf("  Arg [%d][%d]: %s\n", i, j, arguments[i][j]);
//             j++;
//         }
//         i++;
//     }
// }
void print_arguments(t_command_data *data)
{
	int i, j;

	printf("=== Parsed Commands ===\n");
	for (i = 0; i < data->num_commands; i++)
	{
		printf("Command %d:\n", i);
		if (data->arguments && data->arguments[i])
		{
			printf("  Arguments: ");
			for (j = 0; data->arguments[i][j]; j++)
				printf("'%s' ", data->arguments[i][j]);
			printf("\n");
		}
		printf("\n");
	}

	printf("=== Output Redirections (ALL commands) ===\n");
	for (j = 0; j < data->num_out_redirs; j++)
	{
		printf("Redirection %d: file='%s', append=%d\n",
			j, data->out_redirs[j].file, data->out_redirs[j].append);
	}

	if (data->input_file)
		printf("=== Input File: %s ===\n", data->input_file);

	if (data->heredoc_delim)
		printf("=== Heredoc Delimiter: %s ===\n", data->heredoc_delim);

	printf("===========================\n\n");
}




void	execute_commands(t_command_data *data, t_shell *shell)
{
	t_exec_state	state;
	pid_t			*pids;

	state.pipefd[0] = 0;
	state.pipefd[1] = 0;
	state.prev_pipe_read = -1;
	state.heredoc_fd = -1;
	state.i = 0;
	pids = NULL;
	// print_arguments(data);
	if (!data || !data->commands || data->num_commands == 0)
		return ((void)(shell->exit_status = 2));
	parent_builtin(data, &state, shell);
	pids = init_execution(data, &state, shell);
	if (pids == NULL)
		return ;
	while (state.i < data->num_commands)
	{
		run_pipeline(data, &state, shell, pids);
		if (g_signal != 0)
		{
			handle_interrupt_signals(pids, &state, data);
			break ;
		}
	}
	finalize_execution(&state, pids, data, shell);
}
