/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 17:20:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/03 12:59:28 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_builtin(char *command)
{
	if (command == NULL)
		return (0);
	if (ft_strcmp(command, "echo") == 0 || \
		ft_strcmp(command, "pwd") == 0 || \
		ft_strcmp(command, "cd") == 0 || \
		ft_strcmp(command, "export") == 0 || \
		ft_strcmp(command, "unset") == 0 || \
		ft_strcmp(command, "exit") == 0 || \
		ft_strcmp(command, "env") == 0)
		return (1);
	return (0);
}

const char	*get_var_value(const char *name, t_var *vars)
{
	t_var	*var;

	var = vars;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
			return (var->value);
		var = var->next;
	}
	return (NULL);
}

int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || \
		c == '\f' || c == '\r');
}

int	print_exported_env(t_shell *shell)
{
	int		i;
	char	*equal;

	i = 0;
	while (shell->envp[i])
	{
		equal = ft_strchr(shell->envp[i], '=');
		if (equal)
		{
			*equal = '\0';
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(shell->envp[i], STDOUT_FILENO);
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(equal + 1, STDOUT_FILENO);
			ft_putstr_fd("\"\n", STDOUT_FILENO);
			*equal = '=';
		}
		i++;
	}
	return (0);
}

int	handle_invalid_identifier(char *name, t_shell *shell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	shell->exit_status = 1;
	return (1);
}
