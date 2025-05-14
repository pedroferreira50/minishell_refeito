/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:58:37 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/13 19:45:40 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	manage_parent(pid_t pid, pid_t *pids, t_exec_state *state,
	t_command_data *data)
{
	if (pid > 0)
	{
		pids[state->i] = pid;
		if (state->prev_pipe_read != -1)
		{
			close(state->prev_pipe_read);
			state->prev_pipe_read = -1;
		}
		if (state->i < data->num_commands - 1)
		{
			state->prev_pipe_read = state->pipefd[0];
			close(state->pipefd[1]);
		}
		if (state->heredoc_fd != -1)
		{
			close(state->heredoc_fd);
			state->heredoc_fd = -1;
		}
	}
}

void	execute_command(int *i, t_shell *shell, pid_t *pids,
	t_command_data *data)
{
	char	*full_path;

	full_path = find_command_path(data->commands[*i], shell);
	if (full_path == NULL)
	{
		ft_putstr_fd(data->commands[*i], 2);
		ft_putstr_fd(": command not found", 2);
		ft_putstr_fd("\n", 2);
		shell->exit_status = 127;
		g_signal = 127;
		free_command_data(data);
		free(pids);
		finalize_shell(shell);
		exit(127);
	}
	execve(full_path, data->arguments[*i], shell->envp);
	perror("execve");
	free(full_path);
	exit(shell->exit_status);
}

void	run_pipeline(t_command_data *data, t_exec_state *state,
	t_shell *shell, pid_t *pids)
{
	pid_t	pid;

	if (setup_pipeline(data, state, shell) == -1)
	{
		free(pids);
		shell->exit_status = 1;
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		fork_child(data, state, shell, pids);
	}
	if (pid < 0)
	{
		perror("minishell: fork");
		free(pids);
		shell->exit_status = 1;
		return ;
	}
	manage_parent(pid, pids, state, data);
	state->i++;
}
