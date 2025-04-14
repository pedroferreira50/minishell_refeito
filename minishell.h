#ifndef MINISHELL_H
# define MINISHELL_H

# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <string.h>
# include <stdbool.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

extern int g_signal;

typedef struct s_var {
    char *name;
    char *value;
    int exported;      // 0=shell var, 1=env var
    struct s_var *next;
} t_var;

typedef struct s_parse_result {
    char **args;
    char *quote_types;
} t_parse_result;

typedef struct s_command_data {
    char **commands;
    char ***arguments;
    char *heredoc_delim;
    char *input_file;
    char *output_file;
    int append_output;
    int num_commands;
    int num_pipes;
    int heredoc_quoted;
} t_command_data;

typedef struct s_shell {
    char **envp;
    t_var *vars;
    int exit_status;
} t_shell;

typedef struct s_parse {
    char **args;
    int args_count;
    int in_quotes;      // Changed from bool
    char quote_char;
    size_t i;
    size_t start;
    int brace_count;
    const char *cmd;
    char *quote_types;
} t_parse;

typedef struct s_indices {
    size_t i;
    size_t j;
} t_indices;

// main.c
void init_shell(t_shell *shell, char **envp);
char *read_input(void);
int process_input(char *input, t_shell *shell);
void finalize_shell(t_shell *shell);

// signal.c
void setup_signals(void);

// free_handling.c
void free_args(char **args, t_command_data *data);

// env_utils.c
char **copy_envp(char **envp);

// parsing_utils.c
int count_args(char **args);

// string_utils.c
char *ft_strndup(const char *src, size_t n);

//parser_command folder
// parse_command.c
t_parse_result parse_command(const char *cmd, t_shell *shell);
// parse_command_utils.
void initialize_state(t_parse *state, const char *cmd);
int check_errors(t_parse *state, t_shell *shell, int last_was_operator);
void finalize_result(t_parse *state, t_parse_result *result, const char *cmd);
// process_token.c
void process_token(t_parse *state, int *last_was_operator)




#endif