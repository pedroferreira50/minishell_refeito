/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_expansion.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:47:12 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/01 05:13:23 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t calc_var_size(const char *arg, t_indices *indices, t_shell *shell)
{
	size_t	start;
	size_t	var_len;
	char	*var_name;
	char	*var_value;

	if (!arg || !indices || !shell)
		return (0);
	if (arg[indices->i + 1] == '?')
		return handle_exit_status(NULL, 0, &indices->i, shell);
	indices->i++;
	start = indices->i;
	if (!isalpha(arg[start]) && arg[start] != '_')
		return (1);
	while (isalnum(arg[indices->i]) || arg[indices->i] == '_')
		indices->i++;
	if (indices->i == start)
		return (1);
	var_name = ft_strndup(&arg[start], indices->i - start);
	if (!var_name)
		return (0);
	var_value = get_var_value_helper(var_name, shell->vars, shell->envp);
	var_len = ft_strlen(var_value);
	free(var_name);
	return (var_len);
}

size_t	handle_exit_status(char *dest, int fill, size_t *i, t_shell *shell)
{
	char	*status;
	size_t	len;

	status = ft_itoa(shell->exit_status);
	len = ft_strlen(status);
	if (fill && dest)
		ft_strcpy(dest, status);
	free(status);
	*i += 2;
	return (len);
}

char	*get_var_value_helper(const char *name, t_var *vars, char **envp)
{
	t_var	*var;
	int		k;

	var = vars;
	k = 0;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
			return (var->value);
		var = var->next;
	}
	while (envp && envp[k])
	{
		if (ft_strncmp(envp[k], name, ft_strlen(name)) == 0
			&& envp[k][ft_strlen(name)] == '=')
			return (envp[k] + ft_strlen(name) + 1);
		k++;
	}
	return ("");
}

size_t	get_var_len(const char *str, size_t i, t_var *vars, char **envp)
{
	size_t	var_len;
	char	*var_name;
	char	*var_value;

	if (!str)
		return (0);
	var_len = 0;
	while (str[i + 1 + var_len] && (isalnum(str[i + 1 + var_len]) || str[i + 1 + var_len] == '_'))
		var_len++;
	if (var_len == 0)
		return (0);
	var_name = ft_strndup(&str[i + 1], var_len);
	if (!var_name)
		return (0);
	var_value = get_var_value_helper(var_name, vars, envp);
	var_len = ft_strlen(var_value);
	free(var_name);
	return (var_len);
}

char	*expand_variables(const char *arg, char quote_type, t_shell *shell)
{
	size_t	total_size;
	char	*expanded;

	if (!arg)
		return (NULL);
	total_size = calc_expanded_size(arg, quote_type, shell);
	expanded = malloc(total_size);
	if (!expanded)
		return (NULL);
	fill_expanded(expanded, arg, quote_type, shell);
	return (expanded);
}

