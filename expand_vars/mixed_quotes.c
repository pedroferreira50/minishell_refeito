/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mixed_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/01 02:54:30 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/01 02:54:30 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

typedef struct s_quote_segment
{
	char	*content;
	char	quote_type;
	int		start;
	int		end;
}	t_quote_segment;

static int	find_quote_end(const char *str, int start, char quote_char)
{
	int	i;

	i = start + 1;
	while (str[i] && str[i] != quote_char)
		i++;
	return (i);
}

static t_quote_segment	*parse_quote_segments(const char *str, int *count)
{
	t_quote_segment	*segments;
	int				i;
	int				seg_count;
	int				start;
	char			quote_char;

	*count = 0;
	if (!str)
		return (NULL);
	
	// First pass: count segments
	i = 0;
	seg_count = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
		{
			quote_char = str[i];
			i = find_quote_end(str, i, quote_char);
			if (str[i] == quote_char)
				i++;
		}
		else
		{
			while (str[i] && str[i] != '\'' && str[i] != '"')
				i++;
		}
		seg_count++;
	}
	
	segments = malloc(sizeof(t_quote_segment) * seg_count);
	if (!segments)
		return (NULL);
	
	// Second pass: fill segments
	i = 0;
	*count = 0;
	while (str[i])
	{
		start = i;
		if (str[i] == '\'' || str[i] == '"')
		{
			quote_char = str[i];
			i = find_quote_end(str, i, quote_char);
			if (str[i] == quote_char)
				i++;
			segments[*count].quote_type = quote_char;
		}
		else
		{
			while (str[i] && str[i] != '\'' && str[i] != '"')
				i++;
			segments[*count].quote_type = '\0';
		}
		segments[*count].start = start;
		segments[*count].end = i;
		segments[*count].content = ft_strndup(&str[start], i - start);
		(*count)++;
	}
	
	return (segments);
}

static char	*expand_segment(t_quote_segment *segment, t_shell *shell)
{
	char	*expanded;
	char	*content;

	if (!segment || !segment->content)
		return (ft_strdup(""));
	
	if (segment->quote_type == '\'')
	{
		// Single quotes: no expansion, remove quotes
		content = segment->content;
		if (content[0] == '\'' && content[ft_strlen(content) - 1] == '\'')
		{
			// Extract content between quotes without any expansion
			size_t len = ft_strlen(content);
			if (len <= 2)
				return (ft_strdup(""));
			return (ft_strndup(&content[1], len - 2));
		}
		else
			return (ft_strdup(content));
	}
	else if (segment->quote_type == '"')
	{
		// Double quotes: expand variables, remove quotes
		content = segment->content;
		if (content[0] == '"' && content[ft_strlen(content) - 1] == '"')
		{
			char *inner = ft_strndup(&content[1], ft_strlen(content) - 2);
			expanded = expand_variables(inner, '"', shell);
			free(inner);
			return (expanded);
		}
		else
			return (expand_variables(content, '"', shell));
	}
	else
	{
		// No quotes: expand variables
		return (expand_variables(segment->content, '\0', shell));
	}
}

char	*expand_mixed_quotes(const char *str, t_shell *shell)
{
	t_quote_segment	*segments;
	int				count;
	char			*result;
	char			*expanded_segment;
	char			*temp;
	int				i;

	segments = parse_quote_segments(str, &count);
	if (!segments)
		return (ft_strdup(str));
	
	result = ft_strdup("");
	if (!result)
	{
		free(segments);
		return (NULL);
	}
	
	i = 0;
	while (i < count)
	{
		expanded_segment = expand_segment(&segments[i], shell);
		if (expanded_segment)
		{
			temp = result;
			result = ft_strjoin(result, expanded_segment);
			free(temp);
			free(expanded_segment);
		}
		if (segments[i].content)
			free(segments[i].content);
		i++;
	}
	
	free(segments);
	return (result);
}