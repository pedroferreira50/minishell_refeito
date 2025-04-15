#include "minishell.h"

void init_shell(t_shell *shell, char **envp)
{
    shell->envp = copy_envp(envp);
    shell->vars = NULL;
    shell->exit_status = 0;
    g_signal = 0;
    setup_signals();
}

void handle_command(char *input, t_shell *shell)
{
    t_parse_result parsed;

    parsed = parse_command(input, shell);
    free(input);
    if (parsed.args == NULL)
        return;
    expand_and_validate(parsed.args, parsed.quote_types, shell);
    free_args(parsed.args, NULL);
    if (parsed.quote_types != NULL)
        free(parsed.quote_types);
}

int process_input(char *input, t_shell *shell)
{
    if (g_signal == SIGINT)
    {
        shell->exit_status = 130;
        g_signal = 0;
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
    if (input == NULL)
        return (0);
    if (input[0] == '\0')
    {
        free(input);
        return (1);
    }
    add_history(input);
    if (is_var_assignment(input))
    {
        handle_var_assignment(input, shell);
        return (1);
    }
    return (2);
}

void finalize_shell(t_shell *shell)
{
    free_args(shell->envp, NULL);
    free_all_vars(&shell->vars);
    clear_history();
}

int main(int argc, char *argv[], char *envp[])
{
    t_shell shell;
    char *input;

    (void)argc;
    (void)argv;
    init_shell(&shell, envp);
    rl_catch_signals = 0;
    while (1)
    {
        input = readline("minishell> ");
        if (process_input(input, &shell) == 0)
        {
            ft_putstr_fd("exit\n", STDOUT_FILENO);
            break;
        }
        handle_command(input, &shell);
        if (g_signal != 0)
        {
            ft_putstr_fd("exit\n", STDOUT_FILENO);
            break;
        }
    }
    finalize_shell(&shell);
    return (shell.exit_status);
}
