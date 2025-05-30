/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:42:33 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/30 07:25:16 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_operator(t_parse *state, int *last_was_operator,
				t_shell *shell)
{
	char	op;
	int		is_double;

	op = state->cmd[state->i];
	is_double = (state->cmd[state->i + 1] == op && (op == '<' || op == '>'));
	if (is_double)
		state->i += 2;
	else
		state->i++;
	if ((op == '|' && *last_was_operator) || (op == '|' && !state->cmd[state->i]))
	{
		error_operator(op, shell);
		return (0);
	}
	add_argument(state);
	*last_was_operator = 1;
	return (1);
}

static int	check_paren(t_parse *state, t_shell *shell, t_parse_result *result)
{
	if ((state->cmd[state->i] == '(' || state->cmd[state->i] == ')') && \
			!state->in_quotes)
	{
		error_paren(shell);
		free(state->args);
		free(state->quote_types);
		result->args = NULL;
		result->quote_types = NULL;
		return (0);
	}
	return (1);
}

static void	parse_loop(t_parse *state, t_shell *shell, t_parse_result *result,
				int *last_was_operator)
{

	while (state->cmd[state->i])
	{
		while (ft_isspace(state->cmd[state->i]) && !state->in_quotes)
			state->i++;
		if (!state->cmd[state->i])
			break ;
		state->start = state->i;
		if (!check_paren(state, shell, result))
			return ;
		if (!state->in_quotes && (state->cmd[state->i] == '<' || \
				state->cmd[state->i] == '>' || state->cmd[state->i] == '|'))
		{
			if (!handle_operator(state, last_was_operator, shell))
			{
				result->args = NULL;
				result->quote_types = NULL;
				return ;
			}
			continue ;
		}
		process_token(state, last_was_operator);
	}
}

static int	check_initial(const char *cmd, t_shell *shell, t_parse *state)
{
	if (!state->args || !state->quote_types)
	{
		shell->exit_status = 1;
		return (0);
	}
	if (cmd && cmd[0] == '|')
	{
		error_pipe(shell);
		return (0);
	}
	return (1);
}

t_parse_result	parse_command(const char *cmd, t_shell *shell)
{
	t_parse			state;
	t_parse_result	result;
	int				last_was_operator;

	last_was_operator = 0;
	result.args = NULL;
	result.quote_types = NULL;
	initialize_state(&state, cmd);
	if (!check_initial(cmd, shell, &state))
		return (free_state(&state), result);
	parse_loop(&state, shell, &result, &last_was_operator);
	if (!check_errors(&state, shell, last_was_operator))
		return (free_state(&state), result);
	finalize_result(&state, &result, cmd);
	return (result);
}
