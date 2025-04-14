#include "minishell.h"

void error_pipe(t_shell *shell)
{
    ft_putstr_fd("minishell: syntax error near token '|'\n", 2);
    shell->exit_status = 2;
}

void error_paren(t_shell *shell)
{
    ft_putstr_fd("minishell: syntax error near token\n", 2);
    shell->exit_status = 2;
}

void error_operator(char op, t_shell *shell)
{
    ft_putstr_fd("minishell: syntax error near token '", 2);
    ft_putstr_fd(op == '|' ? "|" : (op == '<' ? "<" : ">"), 2);
    ft_putstr_fd("'\n", 2);
    shell->exit_status = 2;
}

void error_quotes(t_shell *shell)
{
    ft_putstr_fd("minishell: syntax error: unclosed quotes\n", 2);
    shell->exit_status = 2;
}

void error_newline(t_shell *shell)
{
    ft_putstr_fd("minishell: syntax error near token 'newline'\n", 2);
    shell->exit_status = 2;
}