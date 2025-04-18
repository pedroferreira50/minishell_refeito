#include "minishell.h"

int count_args(char **args)
{
    int count;

    count = 0;
    while (args[count])
        count++;
    return (count);
}

void add_argument(t_parse *state)
{
    int len;
    size_t max_tokens;

	len = state->i - state->start;
	if (state->cmd != NULL)
    	max_tokens = ft_strlen(state->cmd) / 2 + 2;
	else
    	max_tokens = 4;
    if (state->args_count < 0 || state->args_count >= (int)(max_tokens - 1)) // Leave space for NULL
    {
        ft_putstr_fd("minishell: error: too many tokens\n", STDERR_FILENO);
        return;
    }
    state->args[state->args_count] = ft_strndup(&state->cmd[state->start], len);
    if (state->args[state->args_count])
    {
        if (state->cmd[state->start] == '"' || state->cmd[state->start] == '\'')
    		state->quote_types[state->args_count] = state->cmd[state->start];
		else
    		state->quote_types[state->args_count] = '\0';
        state->args_count++;
    }
    state->start = state->i;
}
