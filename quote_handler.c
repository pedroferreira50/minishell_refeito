#include "minishell.h"

typedef struct s_quote_state {
char quote_char;
int in_quotes;
int brace_count;
} t_quote_state;

static void init_quote_state(t_quote_state *state) {
state->quote_char = '\0';
state->in_quotes = 0;
state->brace_count = 0;
}

static void handle_quote_char(const char *input, size_t *i, t_quote_state *state, char *quote_types, size_t arg_count) {
if ((input[*i] == ''' || input[*i] == '"') && !state->in_quotes) {
state->quote_char = input[*i];
state->in_quotes = 1;
quote_types[arg_count] = state->quote_char;
(*i)++;
} else if (input[*i] == state->quote_char && state->in_quotes && state->brace_count == 0) {
state->in_quotes = 0;
state->quote_char = '\0';
(*i)++;
}
}

static void handle_braces(const char *input, size_t *i, t_quote_state *state) {
if (state->in_quotes) {
if (input[*i] == '{') {
state->brace_count++;
} else if (input[*i] == '}') {
state->brace_count--;
}
(*i)++;
}
}

char **parse_quoted_tokens(const char *input, char **tokens, char *quote_types, size_t *token_count) {
t_quote_state state;
size_t i = 0, start = 0;
size_t max_tokens = input ? ft_strlen(input) / 2 + 2 : 4;

init_quote_state(&state);
*token_count = 0;
tokens = malloc(sizeof(char *) * max_tokens);
quote_types = malloc(sizeof(char) * max_tokens);
if (!tokens || !quote_types) {
free(tokens);
free(quote_types);
return NULL;
}
ft_memset(tokens, 0, sizeof(char *) * max_tokens);
ft_memset(quote_types, 0, sizeof(char) * max_tokens);

while (input[i]) {
while (ft_isspace(input[i]) && !state.in_quotes) {
i++;
start = i;
}
if (!input[i])
break;
if (!state.in_quotes && (input[i] == '<' || input[i] == '>' || input[i] == '|')) {
if (i > start) {
tokens[*token_count] = ft_strndup(&input[start], i - start);
quote_types[*token_count] = '\0';
(*token_count)++;
}
size_t op_len = (input[i + 1] == input[i] && (input[i] == '<' || input[i] == '>')) ? 2 : 1;
tokens[*token_count] = ft_strndup(&input[i], op_len);
quote_types[*token_count] = '\0';
(*token_count)++;
i += op_len;
start = i;
continue;
}
handle_quote_char(input, &i, &state, quote_types, *token_count);
if (state.in_quotes) {
handle_braces(input, &i, &state);
} else {
i++;
}
if (!state.in_quotes && (ft_isspace(input[i]) || !input[i] || input[i] == '<' || input[i] == '>' || input[i] == '|')) {
if (i > start) {
size_t len = i - start;
if (quote_types[*token_count] == '"' || quote_types[*token_count] == ''') {
tokens[*token_count] = ft_strndup(&input[start + 1], len - 2); // Strip quotes
} else {
tokens[*token_count] = ft_strndup(&input[start], len);
}
(*token_count)++;
}
start = i;
}
}
if (i > start) {
size_t len = i - start;
if (quote_types[*token_count] == '"' || quote_types[*token_count] == ''') {
tokens[*token_count] = ft_strndup(&input[start + 1], len - 2); // Strip quotes
} else {
tokens[*token_count] = ft_strndup(&input[start], len);
}
(*token_count)++;
}
tokens[*token_count] = NULL;
return tokens;
}

void strip_quotes(char **tokens, char *quote_types) {
size_t i = 0;
while (tokens[i]) {
if (quote_types[i] == '"' || quote_types[i] == ''') {
char *tmp = tokens[i];
size_t len = ft_strlen(tmp);
if (len >= 2 && tmp[0] == quote_types[i] && tmp[len - 1] == quote_types[i]) {
tokens[i] = ft_strndup(tmp + 1, len - 2);
free(tmp);
}
}
i++;
}
}