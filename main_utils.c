/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:25:17 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/02 07:44:59 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = copy_envp(envp);
	shell->vars = NULL;
	shell->exit_status = 0;
	g_signal = 0;
	setup_signals();
}

void	finalize_shell(t_shell *shell)
{
	free_args(shell->envp, NULL);
	free_all_vars(&shell->vars);
	clear_history();
}

t_shell	*get_shell(void)
{
	static t_shell	shell;

	return (&shell);
}
