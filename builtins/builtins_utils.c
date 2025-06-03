/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 17:20:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:22:39 by pviegas-         ###   ########.fr       */
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
	char	*var_name;
	t_var	*var;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		equal = ft_strchr(shell->envp[i], '=');
		if (equal)
		{
			var_name = ft_substr(shell->envp[i], 0, equal - shell->envp[i]);
			if (!var_name)
				return (1);
			if (strcmp(var_name, "_") != 0)
			{
				*equal = '\0';
				ft_putstr_fd("declare -x ", STDOUT_FILENO);
				ft_putstr_fd(shell->envp[i], STDOUT_FILENO);
				ft_putstr_fd("=\"", STDOUT_FILENO);
				ft_putstr_fd(equal + 1, STDOUT_FILENO);
				ft_putstr_fd("\"\n", STDOUT_FILENO);
				*equal = '=';
			}
			free(var_name);
		}
		i++;
	}
	var = shell->vars;
	while (var)
	{
		if (var->exported && find_env_var_index(shell, var->name) == -1)
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(var->name, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		var = var->next;
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
