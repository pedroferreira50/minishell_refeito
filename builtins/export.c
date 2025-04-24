/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/24 17:30:25 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_invalid_identifier(char *name, t_shell *shell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	shell->exit_status = 1;
	return (1);
}

static void	update_env_var(char *name, char *value, t_shell *shell)
{
	char *temp;
	char *new_entry;
	int index;

	temp = ft_strjoin(name, "=");
	new_entry = ft_strjoin(temp, value);
	free(temp);
	if (!shell->envp)
	{
		shell->envp = malloc(sizeof(char *) * 2);
		if (!shell->envp)
		{
			free(new_entry);
			return;
		}
		shell->envp[0] = ft_strdup(new_entry);
		shell->envp[1] = NULL;
		free(new_entry);
		return;
	}
	index = find_env_var_index(shell, name);
	shell->envp = copy_envp_with_update(shell, new_entry, index);
	free(new_entry);
}
void	add_env_var(t_shell *shell, char *new_entry, char *name)
{
	int		len;
	int		index;

	len = 0;
	while (shell->envp[len])
		len++;
	index = find_env_var_index(shell, name);
	shell->envp = copy_envp_with_update(shell, new_entry, index);
}

void	export_var(char *name, char *value, t_shell *shell)
{
	int		i;
	char	*tmp;
	char	*new_entry;

	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return ;
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_entry)
		return ;
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], name, ft_strlen(name)) == 0
			&& shell->envp[i][ft_strlen(name)] == '=')
		{
			free(shell->envp[i]);
			shell->envp[i] = new_entry;
			return ;
		}
		i++;
	}
	add_env_var(shell, new_entry, name);
	free(new_entry);
}


int	handle_export_with_value(char *arg, t_shell *shell)
{
	char *equal;
	char *name;
	char *value;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (1);
	*equal = '\0';
	name = arg;
	value = equal + 1;
	if (!is_valid_var_name(name))
		return (handle_invalid_identifier(name, shell));
	export_var(name, value, shell);
	return (0);
}



int	handle_export_without_value(char *arg, t_shell *shell)
{
	t_var *var;

	if (!is_valid_var_name(arg))
		return (handle_invalid_identifier(arg, shell));
	var = shell->vars;
	while (var && ft_strcmp(var->name, arg) != 0)
		var = var->next;
	if (var)
	{
		var->exported = 1;
		update_env_var(var->name, var->value, shell);
	}
	else
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not found\n", 2);
		shell->exit_status = 1;
		return (1);
	}
	return (0);
}

int	ft_export(char **args, t_shell *shell)
{
	int i;
	int status;
	char *equal;

	status = 0;
	if (!args[1])
		return (print_exported_env(shell));
	i = 1;
	while (args[i])
	{
		equal = ft_strchr(args[i], '=');
		if (equal)
		{
			status |= handle_export_with_value(args[i], shell);
		}
		else
			status |= handle_export_without_value(args[i], shell);
		i++;
	}
	if (status != 0)
		shell->exit_status = 1;
	return (status);
}
