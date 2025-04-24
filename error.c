/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:23:43 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/24 17:25:20 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error_pipe(t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near token '|'\n", 2);
	shell->exit_status = 2;
}

void	error_paren(t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near token\n", 2);
	shell->exit_status = 2;
}

void	error_operator(char op, t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near token '", 2);
	if (op == '|')
		ft_putstr_fd("|", 2);
	else if (op == '<')
		ft_putstr_fd("<", 2);
	else
		ft_putstr_fd(">", 2);
	ft_putstr_fd("'\n", 2);
	shell->exit_status = 2;
}

void	error_quotes(t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error: unclosed quotes\n", 2);
	shell->exit_status = 2;
}

void	error_newline(t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near token 'newline'\n", 2);
	shell->exit_status = 2;
}
