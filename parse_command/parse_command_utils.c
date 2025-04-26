#include "../minishell.h"

void initialize_state(t_parse *state, const char *cmd)
{
    size_t max_tokens;

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
        return;
    }
    ft_memset(state->args, 0, sizeof(char *) * max_tokens);
    ft_memset(state->quote_types, 0, sizeof(char) * max_tokens);
}

int check_errors(t_parse *state, t_shell *shell, int last_was_operator)
{
    if (state->in_quotes)
    {
        error_quotes(shell);
        return (0);
    }
    if (last_was_operator)
    {
        error_newline(shell);
        return (0);
    }
    return (1);
}

void finalize_result(t_parse *state, t_parse_result *result, const char *cmd)
{
    size_t max_tokens;

    if (cmd)
        max_tokens = ft_strlen(cmd) / 2 + 2;
    else
        max_tokens = 4;
    if (state->args_count < (int)max_tokens)
        state->args[state->args_count] = NULL;
    result->args = state->args;
    result->quote_types = state->quote_types;
}
