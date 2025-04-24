/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 10:59:01 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/24 17:27:43 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

int	find_env_var_index(t_shell *shell, const char *name)
{
	int	i;
	int	len;

	if (!shell->envp || !name)
		return (-1);
	len = ft_strlen(name);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], name, len) == 0 && shell->envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

void	fill_updated_envp(t_shell *shell, char **new_envp, char *new_entry, int replace_index)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (shell->envp[i])
	{
		if (i == replace_index)
			new_envp[j++] = ft_strdup(new_entry);
		else
			new_envp[j++] = ft_strdup(shell->envp[i]);
		free(shell->envp[i]);
		i++;
	}
	if (replace_index == -1)
		new_envp[j++] = ft_strdup(new_entry);
	new_envp[j] = NULL;
}

char	**copy_envp_with_update(t_shell *shell, char *new_entry, int replace_index)
{
	int		count;
	int		new_size;
	char	**new_envp;

	count = 0;
	while (shell->envp && shell->envp[count])
		count++;
	if (replace_index == -1)
		new_size = count + 2;
	else
		new_size = count + 1;
	new_envp = malloc(sizeof(char *) * new_size);
	if (!new_envp)
		return (NULL);
	fill_updated_envp(shell, new_envp, new_entry, replace_index);
	free(shell->envp);
	return (new_envp);
}
