/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_execute_builtins.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/23 19:16:33 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void restore_fds(int original_stdin, int original_stdout)
{
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdin);
    close(original_stdout);
}

int handle_input_redirection(t_command_data *data, int *i, int original_stdin, t_shell *shell)
{
    int fd_in;

    fd_in = -1;
    if (*i == 0 && data->input_file)
    {
        fd_in = open(data->input_file, O_RDONLY);
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

int handle_output_redirection(t_command_data *data, int *i, int original_stdout, t_shell *shell)
{
    int fd_out;

    fd_out = -1;
    if (*i == data->num_commands - 1 && data->output_file)
    {
        if (data->append_output)
            fd_out = open(data->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd_out = open(data->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
            perror("open output file");
            shell->exit_status = 1;
            restore_fds(STDIN_FILENO, original_stdout);
            return (1);
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
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


int child_builtin(int *i, t_shell *shell, t_command_data *data)
{
    int original_stdin;
    int original_stdout;
    char *command;
    char **args;

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
