/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:24:26 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:16:44 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

void	signal_reset_prompt(int signo)
{
	(void)signo;
	write(1, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	signal_print_newline(int signal)
{
	(void)signal;
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
}

void	ignore_sigquit(void)
{
	struct sigaction	act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &act, NULL);
}

void	set_signals_interactive(void)
{
	struct sigaction	act;

	ignore_sigquit();
	memset(&act, 0, sizeof(act));
	act.sa_handler = &signal_reset_prompt;
	sigaction(SIGINT, &act, NULL);
}

void	set_signals_noninteractive(void)
{
	struct sigaction	act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = &signal_print_newline;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
}

void	setup_signals(void)
{
	set_signals_interactive();
}

