/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:23:43 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:57:52 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error_operator(char op, t_shell *shell)
{
	if (op == '|')
		print_error_token("|", 2, shell);
	else if (op == '<')
		print_error_token("<", 2, shell);
	else
		print_error_token(">", 2, shell);
}


void	print_error_simple(const char *message, int exit_code, t_shell *shell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	shell->exit_status = exit_code;
}

void	print_error_command(const char *command, const char *message,
	int exit_code, t_shell *shell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)command, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	shell->exit_status = exit_code;
}

void	print_error_token(const char *token, int exit_code, t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near token '", 2);
	ft_putstr_fd((char *)token, 2);
	ft_putstr_fd("'\n", 2);
	shell->exit_status = exit_code;
}

void	print_error_and_exit(const char *command, const char *message,
	int exit_code, t_shell *shell, t_command_data *data, pid_t *pids)
{
	ft_putstr_fd("minishell: ", 2);
	if (command)
	{
		ft_putstr_fd((char *)command, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	shell->exit_status = exit_code;
	g_signal = exit_code;
	if (data)
		free_command_data(data);
	if (pids)
		free(pids);
	finalize_shell(shell);
	exit(exit_code);
}
