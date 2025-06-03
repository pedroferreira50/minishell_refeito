/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:36:00 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:58:30 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_state(t_parse *state)
{
	if (state->args)
	{
		free(state->args);
		state->args = NULL;
	}
	if (state->quote_types)
	{
		free(state->quote_types);
		state->quote_types = NULL;
	}
}

void	initialize_state(t_parse *state, const char *cmd)
{
	size_t	max_tokens;

	if (cmd != NULL)
		max_tokens = ft_strlen(cmd) / 2 + 2;
	else
		max_tokens = 4;
	ft_memset(state, 0, sizeof(t_parse));
	state->cmd = cmd;
	state->args = malloc(sizeof(char *) * max_tokens);
	state->quote_types = malloc(sizeof(char) * max_tokens);
	if (!state->args || !state->quote_types)
	{
		if (state->args)
			free(state->args);
		if (state->quote_types)
			free(state->quote_types);
		state->args = NULL;
		state->quote_types = NULL;
		return ;
	}
	ft_memset(state->args, 0, sizeof(char *) * max_tokens);
	ft_memset(state->quote_types, 0, sizeof(char) * max_tokens);
}

int	check_errors(t_parse *state, t_shell *shell, int last_was_operator)
{
	if (state->in_quotes)
	{
		print_error_simple("syntax error: unclosed quotes", 2, shell);
		return (0);
	}
	if (last_was_operator)
	{
		print_error_token("newline", 2, shell);
		return (0);
	}
	return (1);
}

void	finalize_result(t_parse *state, t_parse_result *result, const char *cmd)
{
	size_t	max_tokens;

	if (cmd)
		max_tokens = ft_strlen(cmd) / 2 + 2;
	else
		max_tokens = 4;
	if (state->args_count < (int)max_tokens)
		state->args[state->args_count] = NULL;
	result->args = state->args;
	result->quote_types = state->quote_types;
}
