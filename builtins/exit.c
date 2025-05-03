/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:05:58 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/03 13:13:51 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	print_numeric_error(const char *arg, t_shell *shell)
{
	char	*msg;
	char	*tmp;

	tmp = ft_strjoin("minishell: exit: ", arg);
	msg = ft_strjoin(tmp, ": numeric argument required\n");
	free(tmp);
	ft_putstr_fd(msg, STDERR_FILENO);
	free(msg);
	shell->exit_status = 2;
	finalize_shell(shell);
	exit(2);
}

void	ft_exit(char **args, t_shell *shell)
{
	int	exit_code;

	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (args[1])
	{
		if (!is_numeric(args[1]))
			print_numeric_error(args[1], shell);
		if (args[2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", \
				STDERR_FILENO);
			shell->exit_status = 1;
			return ;
		}
		exit_code = ft_atoi(args[1]) % 256;
	}
	else
		exit_code = shell->exit_status;
	shell->exit_status = exit_code;
	finalize_shell(shell);
	exit(exit_code);
}
