/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 15:03:27 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/03 11:28:35 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_interrupt_signals(pid_t *pids, t_exec_state *state,
	t_command_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_commands)
	{
		if (pids[i] > 0)
			kill(pids[i], SIGINT);
		i++;
	}
	if (state->prev_pipe_read != -1)
	{
		close(state->prev_pipe_read);
		state->prev_pipe_read = -1;
	}
}

void	handle_wait_status(int status, t_shell *shell)
{
	if (WIFEXITED(status))
	{
		shell->exit_status = WEXITSTATUS(status);
	}
	else
	{
		if (WIFSIGNALED(status))
		{
			shell->exit_status = 128 + WTERMSIG(status);
			if (shell->exit_status == 130)
				ft_putstr_fd("\n", 2);
			if (shell->exit_status == 139)
				ft_putstr_fd("minishell: segmentation fault\n", 2);
		}
	}
}

void	wait_commands(pid_t *pids, t_command_data *data, t_shell *shell)
{
	int	i;
	int	status;

	i = 0;
	while (i < data->num_commands)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, 0) > 0)
				handle_wait_status(status, shell);
		}
		i = i + 1;
	}
	free(pids);
}

void	fork_child(t_command_data *data, t_exec_state *state,
	t_shell *shell, pid_t *pids)
{
	int	has_builtin;

	if (data->commands[state->i] == NULL)
		exit(0);
	has_builtin = check_builtin(data->commands[state->i]);
	if (state->heredoc_fd != -1)
	{
		dup2(state->heredoc_fd, STDIN_FILENO);
		close(state->heredoc_fd);
	}
	setup_pipes_and_redirections(data, state, data->num_commands, shell);
	if (state->i < data->num_commands - 1)
		close(state->pipefd[0]);
	if (has_builtin && get_shell()->is_save_to_execute == true)
	{
		child_builtin(&state->i, shell, data);
		free_command_data(data);
		free_args(shell->envp, NULL);
		free_all_vars(&shell->vars);
		exit(shell->exit_status);
	}
	if (!has_builtin)
		execute_command(&state->i, shell, pids, data);
	free_command_data(data);
	exit(1);
}
