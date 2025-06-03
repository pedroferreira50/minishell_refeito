/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:35:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:26:26 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"



static void	handle_quotes(t_parse *state)
{
	if ((state->cmd[state->i] == '\'' || state->cmd[state->i] == '"') && \
			!state->in_quotes)
	{
		state->quote_char = state->cmd[state->i];
		state->in_quotes = 1;
	}
	else if (state->cmd[state->i] == state->quote_char && \
				state->in_quotes && state->brace_count == 0)
	{
		state->in_quotes = 0;
		state->quote_char = '\0';
	}
}

static void	handle_braces(t_parse *state)
{
	if (state->in_quotes)
	{
		if (state->cmd[state->i] == '{')
			state->brace_count++;
		else if (state->cmd[state->i] == '}')
			state->brace_count--;
		state->i++;
	}
}

void	process_token(t_parse *state, int *last_was_operator)
{
	int	had_quotes;
	
	had_quotes = 0;
	while (state->cmd[state->i] && (!ft_isspace(state->cmd[state->i]) \
		|| state->in_quotes))
	{
		if ((state->cmd[state->i] == '\'' || state->cmd[state->i] == '"') && !state->in_quotes)
			had_quotes = 1;
		handle_quotes(state);
		if (!state->in_quotes && (state->cmd[state->i] == '<' || \
				state->cmd[state->i] == '>' || state->cmd[state->i] == '|'))
			break ;
		if (state->in_quotes)
			handle_braces(state);
		else
			state->i++;
	}
	if (state->i > state->start || had_quotes)
	{
		add_argument(state);
		*last_was_operator = 0;
	}
}
