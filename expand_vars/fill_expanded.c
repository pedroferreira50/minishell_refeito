/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_expanded.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:44:40 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/13 19:55:53 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t calc_expanded_size(const char *arg, char quote_type, t_shell *shell)
{
    size_t size;
    t_indices indices;

	size = 0;
	indices.i = 0;
	indices.j = 0;
    if (quote_type == '\'')
        return (ft_strlen(arg) + 1);
    while (arg[indices.i])
    {
        if (arg[indices.i] == '$')
        {
            size += calc_var_size(arg, &indices, shell);
            continue;
        }
        size++;
        indices.i++;
    }
    return (size + 1);
}

static void fill_quotes(char *dest, const char *src, t_indices *indices, char quote_type)
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

static void fill_exit_status(char *dest, t_indices *indices, t_shell *shell)
{
    size_t len;

	len = handle_exit_status(&dest[indices->j], 1, &indices->i, shell);
    indices->j += len;
}

static void fill_var_name(char *dest, const char *src, t_indices *indices, t_shell *shell)
{
	size_t	start;
	char	*key;
	char	*val;

	indices->i++;
	start = indices->i;
	if (!isalpha(src[start]) && src[start] != '_')
	{
		dest[indices->j++] = '$';
		return;
	}
	while (isalnum(src[indices->i]) || src[indices->i] == '_')
		indices->i++;
	key = ft_strndup(&src[start], indices->i - start);
	if (!key)
		return;
	val = get_var_value_helper(key, shell->vars, shell->envp);
	if (val)
	{
		ft_strcpy(&dest[indices->j], val);
		indices->j += ft_strlen(val);
	}
	free(key);
}



void fill_expanded(char *dest, const char *src, char quote_type, t_shell *shell)
{
    t_indices indices;

    indices.i = 0;
    indices.j = 0;
    if (!dest || !src)
        return;

    if (quote_type == '\'')
        return fill_quotes(dest, src, &indices, quote_type);

    while (src[indices.i])
    {
        if (src[indices.i] == '$')
        {
            // Expansão de $? (exit status)
            if (src[indices.i + 1] == '?')
                fill_exit_status(dest, &indices, shell);
            // Expansão de variáveis válidas (letras ou _)
            else if (ft_isalpha(src[indices.i + 1]) || src[indices.i + 1] == '_')
                fill_var_name(dest, src, &indices, shell);
            // Qualquer outro caractere: copia $ como literal
            else
                dest[indices.j++] = src[indices.i++];
        }
        else
        {
            // Cópia literal de outros caracteres
            dest[indices.j++] = src[indices.i++];
        }
    }
    dest[indices.j] = '\0';
}
