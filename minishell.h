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

typedef struct s_exec_state {
    int pipefd[2];
    int prev_pipe_read;
    int heredoc_fd;
    int i;
} t_exec_state;

typedef struct s_indices {
    size_t i;
    size_t j;
} t_indices;

typedef struct s_parse_state {
    t_indices idx;
    int command_index;
} t_parse_state;

// main.c
void init_shell(t_shell *shell, char **envp);
char *read_input(void);
int process_input(char *input, t_shell *shell);
void finalize_shell(t_shell *shell);

// signal.c
void setup_signals(void);

// free_handling.c
void free_args(char **args, t_command_data *data);
void free_command_data(t_command_data *data);


//handle_operator.c

void handle_pipe(t_command_data *data, int *command_index, t_shell *shell);
void handle_redirect(char **args, t_command_data *data, t_indices *indices, t_shell *shell);
void handle_heredoc(char **args, t_command_data *data, t_indices *indices, t_shell *shell);

// env_utils.c
char **copy_envp(char **envp);

// parsing_utils.c
int count_args(char **args);
void add_argument(t_parse *state);

// string_utils.c
char *ft_strndup(const char *src, size_t n);

//error.c
void error_newline(t_shell *shell);
void error_quotes(t_shell *shell);
void error_operator(char op, t_shell *shell);
void error_paren(t_shell *shell);
void error_pipe(t_shell *shell);

//parser_command folder
// parse_command.c
t_parse_result parse_command(const char *cmd, t_shell *shell);
// parse_command_utils.
void initialize_state(t_parse *state, const char *cmd);
int check_errors(t_parse *state, t_shell *shell, int last_was_operator);
void finalize_result(t_parse *state, t_parse_result *result, const char *cmd);
// process_token.c
void process_token(t_parse *state, int *last_was_operator);
//parse_input_utils.c
int is_operator(const char *token);
//parse_input.c
void parse_input(char **args, int count, t_command_data *data, t_shell *shell);

//builtins
// check_execute_builtins.c
int		child_builtin(int *i, t_shell *shell, t_command_data *data);
void	parent_builtin(t_command_data *data, t_exec_state *state, t_shell *shell);

// builtins_utils.c
int ft_isspace(int c);
int	check_builtin(char *command);
//cd
int	ft_cd(char **args, int *i, t_shell *shell);
//echo
void	ft_echo(char **args, t_shell *shell);
//env
void	ft_env(char **envp, t_shell *shell);
//export
int ft_export(char **args, t_shell *shell);
int print_exported_env(t_shell *shell);
int find_env_var_index(t_shell *shell, const char *name);
char **copy_envp_with_update(t_shell *shell, char *new_entry, int replace_index);
//pwd
int	ft_pwd(void);
//unset
int	ft_unset(char **args, t_var **vars, char ***envp);
//exit
void ft_exit(char **args, t_shell *shell);
//loc_vars
int is_valid_var_name(const char *name);
void store_var(char *name, char *value, t_var **vars);
void free_all_vars(t_var **vars);
int is_var_assignment(const char *input);
void handle_var_assignment(const char *input, t_shell *shell);

//expand_vars folder
// fill_expand.c
size_t calc_expanded_size(const char *arg, char quote_type, t_shell *shell);
void fill_expanded(char *dest, const char *src, char quote_type, t_shell *shell);
// var_expansion.c
size_t handle_exit_status(char *dest, int fill, size_t *i, t_shell *shell);
size_t get_var_len(const char *str, size_t i, t_var *vars, char **envp);
char *get_var_value_helper(const char *name, t_var *vars, char **envp);
char *expand_variables(const char *arg, char quote_type, t_shell *shell);

//find_command_path.c
char *find_command_path(char *command, t_shell *shell);

//pipes_and_execution folder
// pipeline.c
void setup_pipes_and_redirections(t_command_data *data, t_exec_state *state, int num_commands, t_shell *shell);
int setup_pipeline(t_command_data *data, t_exec_state *state, t_shell *shell);
// heredoc.c
int setup_heredoc(const char *delimiter, t_shell *shell, int suppress_expansion);
//  execute_commands.c
void execute_commands(t_command_data *data, t_shell *shell);

//parser.c
void expand_and_validate(char **tokens, char *quote_types, t_shell *shell);






#endif
