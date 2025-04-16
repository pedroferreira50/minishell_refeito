/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:05:58 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/16 07:14:04 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int is_numeric(const char *str)
{
    int i;

    i = 0;
    if (str[i] == '+' || str[i] == '-')
        i++;
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
            return (0);
        i++;
    }
    return (1);
}

void ft_exit(char **args, t_shell *shell)
{
    int exit_code;

    ft_putstr_fd("exit\n", STDOUT_FILENO);
    if (args[1])
    {
        if (!is_numeric(args[1]))
        {
            ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
            ft_putstr_fd(args[1], STDERR_FILENO);
            ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
            shell->exit_status = 2;
            finalize_shell(shell);
            exit(2);
        }
        if (args[2])
        {
            ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
            shell->exit_status = 1;
            return;
        }
        exit_code = ft_atoi(args[1]);
        exit_code = exit_code % 256;
    }
    else
        exit_code = shell->exit_status;
    shell->exit_status = exit_code;
    finalize_shell(shell);
    exit(exit_code);
}
