/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_execute_builtins.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/30 05:49:10 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	restore_fds(int original_stdin, int original_stdout)
{
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
}

int	handle_input_redirection(t_command_data *data, int *i, int original_stdin,
		t_shell *shell)
{
	int	fd_in;

	fd_in = -1;
	if (data->input_files && data->input_files[*i])
	{
		fd_in = open(data->input_files[*i], O_RDONLY);
		if (fd_in == -1)
		{
			perror("open input file");
			shell->exit_status = 1;
			restore_fds(original_stdin, STDOUT_FILENO);
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	return (0);
}

int	handle_output_redirection(t_command_data *data, int *i, int original_stdout, t_shell *shell)
{
	int	fd_out;
	int	j;
	int	flags;

	j = 0;
	if (data->num_out_redirs && data->num_out_redirs[*i] > 0)
	{
		while (j < data->num_out_redirs[*i])
		{
			flags = O_WRONLY | O_CREAT;
			if (data->out_redirs[*i][j].append)
				flags = flags | O_APPEND;
			else
				flags = flags | O_TRUNC;
			fd_out = open(data->out_redirs[*i][j].file, flags, 0644);
			if (fd_out == -1)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(data->out_redirs[*i][j].file, 2);
				ft_putstr_fd(": Failed to open file\n", 2);
				shell->exit_status = 1;
				restore_fds(STDIN_FILENO, original_stdout);
				return (1);
			}
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
			j++;
		}
	}
	return (0);
}

int	execute_builtin_command(char *command, char **args, t_shell *shell, int *i)
{
	if (ft_strcmp(command, "echo") == 0)
		return (ft_echo(args, shell), 1);
	if (ft_strcmp(command, "pwd") == 0)
		return (ft_pwd(), 1);
	if (ft_strcmp(command, "env") == 0)
		return (ft_env(shell->envp, shell), 1);
	if (ft_strcmp(command, "cd") == 0)
		return (ft_cd(args, i, shell), 1);
	if (ft_strcmp(command, "export") == 0)
		return (ft_export(args, shell), 1);
	if (ft_strcmp(command, "unset") == 0)
		return (ft_unset(args, &shell->vars, &shell->envp), 1);
	if (ft_strcmp(command, "exit") == 0)
		return (ft_exit(args, shell), 1);
	return (0);
}

int	child_builtin(int *i, t_shell *shell, t_command_data *data)
{
	int		original_stdin;
	int		original_stdout;
	char	*command;
	char	**args;

	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	command = data->commands[*i];
	args = data->arguments[*i];
	if (handle_input_redirection(data, i, original_stdin, shell))
		return (1);
	if (handle_output_redirection(data, i, original_stdout, shell))
		return (1);
	execute_builtin_command(command, args, shell, i);
	restore_fds(original_stdin, original_stdout);
	return (1);
}
