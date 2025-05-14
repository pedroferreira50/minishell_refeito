/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:35:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/14 11:21:57 by scarlos-         ###   ########.fr       */
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
		//state->i++; //falha em caso de teste''teste
	}
	else if (state->cmd[state->i] == state->quote_char && \
				state->in_quotes && state->brace_count == 0)
	{
		state->in_quotes = 0;
		state->quote_char = '\0';
		// state->i++; //cat < '1.txt''2.txt' -> unclosed quotes
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
	while (state->cmd[state->i] && (!ft_isspace(state->cmd[state->i]) \
		|| state->in_quotes))
	{
		handle_quotes(state);
		if (!state->in_quotes && (state->cmd[state->i] == '<' || \
				state->cmd[state->i] == '>' || state->cmd[state->i] == '|'))
			break ;
		if (state->in_quotes)
			handle_braces(state);
		else
			state->i++;
	}
	if (state->i > state->start)
	{
		add_argument(state);
		*last_was_operator = 0;
	}
}
