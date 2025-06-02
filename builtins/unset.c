/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:13 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/02 02:28:01 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	remove_from_vars(char *name, t_var **vars)
{
	t_var	*current;
	t_var	*prev;

	if (!name || !vars || !*vars)
		return ;
	current = *vars;
	prev = NULL;
	while (current)
	{
		if (current->name && ft_strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*vars = current->next;
			if (current->name)
				free(current->name);
			if (current->value)
				free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	shift_envp(char ***envp, int start_idx, int len)
{
	int	i;

	i = start_idx;
	while (i < len - 1)
	{
		(*envp)[i] = (*envp)[i + 1];
		i++;
	}
	(*envp)[len - 1] = NULL;
}

static void	remove_from_env(char *name, char ***envp)
{
	int	i;
	int	len;

	if (!name || !envp || !*envp)
		return ;
	i = 0;
	len = 0;
	while ((*envp)[len])
		len++;
	while (i < len && (*envp)[i])
	{
		if ((*envp)[i] && ft_strncmp((*envp)[i], name, ft_strlen(name)) == 0
			&& (*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			shift_envp(envp, i, len);
			return ;
		}
		i++;
	}
}

/* static bool	is_invalid_arg(char *arg)
{
	if (!is_valid_var_name(arg))
	{
		return (1);
	}
	return (0);
} */

int	ft_unset(char **args, t_var **vars, char ***envp)
{
	int	i;

	if (!args || !vars || !envp)
		return (1);
	i = 1;
	while (args[i])
	{
		if (args[i][0] == '\0')
		{
			i++;
			continue;
		}
		// Check for invalid options (arguments starting with -)
		if (args[i][0] == '-' && args[i][1] != '\0')
		{
			ft_putstr_fd("minishell: unset: -", 2);
			ft_putchar_fd(args[i][1], 2);
			ft_putstr_fd(": invalid option\n", 2);
			ft_putstr_fd("unset: usage: unset [name ...]\n", 2);
			return (2);
		}
		if (!is_valid_var_name(args[i]))
		{
			// Bash silently ignores invalid variable names in unset
			i++;
			continue;
		}
		remove_from_vars(args[i], vars);
		remove_from_env(args[i], envp);
		i++;
	}
	return (0);
}
