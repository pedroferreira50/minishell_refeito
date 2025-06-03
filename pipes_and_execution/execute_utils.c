/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:58:37 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:45:21 by pviegas-         ###   ########.fr       */
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

static int	path_is_unset_or_empty(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
		{
			if (shell->envp[i][5] == '\0')
				return (1);
			return (0);
		}
		i++;
	}
	return (1);
}

void	execute_command(int *i, t_shell *shell, pid_t *pids,
	t_command_data *data)
{
	char		*full_path;
	struct stat	sb;

	full_path = find_command_path(data->commands[*i], shell);
	if (full_path == NULL)
	{
		if (!ft_strchr(data->commands[*i], '/') && path_is_unset_or_empty(shell) &&
			stat(data->commands[*i], &sb) == 0 && !S_ISDIR(sb.st_mode))
		{
			print_error_command(data->commands[*i], "Permission denied", 126, shell);
			g_signal = 126;
			free_command_data(data);
			free(pids);
			finalize_shell(shell);
			exit(126);
		}
		print_error_command(data->commands[*i], "command not found", 127, shell);
		g_signal = 127;
		free_command_data(data);
		free(pids);
		finalize_shell(shell);
		exit(127);
	}
	execve(full_path, data->arguments[*i], shell->envp);
	perror("execve");
	free(full_path);
	free_command_data(data);
	free(pids);
	finalize_shell(shell);
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
