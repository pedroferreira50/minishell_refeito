/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/02 04:53:16 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	update_env_var(char *name, char *value, t_shell *shell)
{
	char	*temp;
	char	*new_entry;
	int		index;

	temp = ft_strjoin(name, "=");
	new_entry = ft_strjoin(temp, value);
	free(temp);
	if (!shell->envp)
	{
		shell->envp = malloc(sizeof(char *) * 2);
		if (!shell->envp)
		{
			free(new_entry);
			return ;
		}
		shell->envp[0] = ft_strdup(new_entry);
		shell->envp[1] = NULL;
		free(new_entry);
		return ;
	}
	index = find_env_var_index(shell, name);
	shell->envp = copy_envp_with_update(shell, new_entry, index);
	free(new_entry);
}

void	export_var(char *name, char *value, t_shell *shell)
{
	int		i;
	char	*tmp;
	char	*new_entry;

	if (!name || !value || !shell)
		return ;
	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return ;
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!new_entry)
		return ;
	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], name, ft_strlen(name)) == 0 && \
				shell->envp[i][ft_strlen(name)] == '=')
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

void	export_var_append(char *name, char *value, t_shell *shell)
{
	char	*existing_value;
	char	*new_value;
	
	if (!name || !value || !shell)
		return ;
	
	// Get existing value from environment or local variables
	existing_value = get_var_value_helper(name, shell->vars, shell->envp);
	if (!existing_value)
		existing_value = "";
	
	// Concatenate existing value with new value
	new_value = ft_strjoin(existing_value, value);
	if (!new_value)
		return ;
	
	// Export the concatenated value
	export_var(name, new_value, shell);
	free(new_value);
}

int	handle_export_with_value(char *arg, t_shell *shell)
{
	char	*equal;
	char	*name;
	char	*value;
	int		is_append;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (1);
	
	// Check for += operator
	is_append = (equal > arg && *(equal - 1) == '+');
	if (is_append)
		*(equal - 1) = '\0';  // Remove the + from name
	else
		*equal = '\0';
	
	name = arg;
	value = equal + 1;
	if (!is_valid_var_name(name))
		return (handle_invalid_identifier(name, shell));
	
	if (is_append)
		export_var_append(name, value, shell);
	else
		export_var(name, value, shell);
	return (0);
}

int	handle_export_without_value(char *arg, t_shell *shell)
{
	t_var	*var;

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
		update_env_var(arg, "", shell);
	return (0);
}

void insertion_sort_env(char **env)
{
	int		i;
	int		j;
	char	*key;

	i = 1;
	while (env[i])
	{
		key = env[i];
		j = i - 1;
		while (j >= 0 && ft_strcmp(env[j], key) > 0)
		{
			env[j + 1] = env[j];
			j--;
		}
		env[j + 1] = key;
		i++;
	}
}

int	ft_export(char **args, t_shell *shell)
{
	int		i;
	int		status;
	char	*equal;

	status = 0;
	if (!args[1])
	{
		insertion_sort_env(shell->envp);
		return (print_exported_env(shell));
	}
	i = 1;
	while (args[i])
	{
		// Check for invalid options (arguments starting with -)
		if (args[i][0] == '-' && args[i][1] != '\0')
		{
			ft_putstr_fd("minishell: export: -", 2);
			ft_putchar_fd(args[i][1], 2);
			ft_putstr_fd(": invalid option\n", 2);
			ft_putstr_fd("export: usage: export [name[=value] ...]\n", 2);
			return (2);
		}
		equal = ft_strchr(args[i], '=');
		if (equal)
		{
			status |= handle_export_with_value(args[i], shell);
		}
		else
		{
			status |= handle_export_without_value(args[i], shell);
		}
		i++;
	}
	shell->exit_status = 0;
	return (status);
}
