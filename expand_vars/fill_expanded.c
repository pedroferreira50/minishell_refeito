/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_expanded.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:44:40 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/01 05:13:07 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	calc_expanded_size(const char *arg, char quote_type, t_shell *shell)
{
	size_t		size;
	t_indices	indices;

	if (!arg || !shell)
		return (1);
	size = 0;
	indices.i = 0;
	indices.j = 0;
	if (quote_type == '\'')
		return (ft_strlen(arg) + 1);
	while (arg[indices.i])
	{
		if (arg[indices.i] == '\\' && arg[indices.i + 1])
		{
			// Handle escaped characters
			if (arg[indices.i + 1] == '$')
			{
				size++; // Just the $ character
				indices.i += 2; // Skip both \ and $
			}
			else if (arg[indices.i + 1] == '\\')
			{
				size++; // One backslash for each pair
				indices.i += 2; // Skip both backslashes
			}
			else
			{
				size += 2; // Keep both \ and the next character
				indices.i += 2;
			}
			continue;
		}
		if (arg[indices.i] == '$')
		{
			// Check for $"..." syntax - should not expand variables inside
			if (arg[indices.i + 1] == '"')
			{
				// Find the closing quote and count the literal content
				size_t start = indices.i + 2;
				size_t end = start;
				while (arg[end] && arg[end] != '"')
					end++;
				if (arg[end] == '"')
				{
					size += (end - start); // Just the content inside quotes
					indices.i = end + 1;
				}
				else
				{
					// No closing quote, treat as regular $
					size += calc_var_size(arg, &indices, shell);
				}
			}
			else
			{
				size += calc_var_size(arg, &indices, shell);
			}
			continue ;
		}
		size++;
		indices.i++;
	}
	return (size + 1);
}

static void	fill_quotes(char *dest, const char *src, t_indices *indices,
	char quote_type)
{
	indices->i++;
	while (src[indices->i] && src[indices->i] != quote_type)
	{
		dest[indices->j++] = src[indices->i++];
	}
	if (src[indices->i] == quote_type)
		indices->i++;
	dest[indices->j] = '\0';
}

static void	fill_exit_status(char *dest, t_indices *indices, t_shell *shell)
{
	size_t	len;

	len = handle_exit_status(&dest[indices->j], 1, &indices->i, shell);
	indices->j += len;
}

static void	fill_var_name(char *dest, const char *src, t_indices *indices,
	t_shell *shell)
{
	size_t	start;
	char	*key;
	char	*val;

	if (!dest || !src || !indices || !shell)
		return ;
	indices->i++;
	start = indices->i;
	if (!isalpha(src[start]) && src[start] != '_')
	{
		dest[indices->j++] = '$';
		return ;
	}
	while (isalnum(src[indices->i]) || src[indices->i] == '_')
		indices->i++;
	if (indices->i == start)
	{
		dest[indices->j++] = '$';
		return ;
	}
	key = ft_strndup(&src[start], indices->i - start);
	if (!key)
		return ;
	val = get_var_value_helper(key, shell->vars, shell->envp);
	if (val)
	{
		ft_strcpy(&dest[indices->j], val);
		indices->j += ft_strlen(val);
	}
	free(key);
}



void	fill_expanded(char *dest, const char *src, char quote_type, t_shell *shell)
{
	t_indices	indices;

	indices.i = 0;
	indices.j = 0;
	if (!dest || !src)
		return ;
	if (quote_type == '\'')
		return (fill_quotes(dest, src, &indices, quote_type));
	while (src[indices.i])
	{
		if (src[indices.i] == '\\' && src[indices.i + 1])
		{
			// Handle escaped characters
			if (src[indices.i + 1] == '$')
			{
				// Escaped $: output literal $
				dest[indices.j++] = '$';
				indices.i += 2; // Skip both \ and $
			}
			else if (src[indices.i + 1] == '\\')
			{
				// Escaped backslash: output one backslash
				dest[indices.j++] = '\\';
				indices.i += 2; // Skip both backslashes
			}
			else
			{
				// Other escaped characters: keep both \ and the character
				dest[indices.j++] = src[indices.i++];
				dest[indices.j++] = src[indices.i++];
			}
		}
		else if (src[indices.i] == '$')
		{
			// Check for $"..." syntax
			if (src[indices.i + 1] == '"')
			{
				// Find the closing quote and copy literal content
				size_t start = indices.i + 2;
				size_t end = start;
				while (src[end] && src[end] != '"')
					end++;
				if (src[end] == '"')
				{
					// Copy the content inside quotes literally
					while (start < end)
						dest[indices.j++] = src[start++];
					indices.i = end + 1;
				}
				else
				{
					// No closing quote, treat as regular $
					if (src[indices.i + 1] == '?')
						fill_exit_status(dest, &indices, shell);
					else if (ft_isalpha(src[indices.i + 1]) || src[indices.i + 1] == '_')
						fill_var_name(dest, src, &indices, shell);
					else
						dest[indices.j++] = src[indices.i++];
				}
			}
			else if (src[indices.i + 1] == '?')
				fill_exit_status(dest, &indices, shell);
			else if (ft_isalpha(src[indices.i + 1]) || src[indices.i + 1] == '_')
				fill_var_name(dest, src, &indices, shell);
			else
				dest[indices.j++] = src[indices.i++];
		}
		else
		{
			dest[indices.j++] = src[indices.i++];
		}
	}
	dest[indices.j] = '\0';
}
