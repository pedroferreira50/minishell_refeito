/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:05:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/21 17:32:49 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_assignment_non_export(char *input, t_parse_result *parsed)
{
	if (input != NULL)
		free(input);
	free_args(parsed->args, NULL);
	free(parsed->quote_types);
}

void handle_command(char *input, t_shell *shell)
{
    t_parse_result parsed;
    t_command_data data;
    char **expanded_args;
    char **filtered_args;
    int i;
	int j;
	int count;

    parsed = parse_command(input, shell);
    if (ft_strchr(input, '=') && ft_strcmp(parsed.args[0], "export") != 0)
	return (handle_assignment_non_export(input, &parsed), (void)0);
    if (!parsed.args)
	return;
    expanded_args = expand_tokens(parsed.args, parsed.quote_types, shell);

    if (!expanded_args)
    {
        free_args(parsed.args, NULL);
        free(parsed.quote_types);
        return;
    }
    count = 0;
	i = 0;
    while (expanded_args[i])
	{
    	if (expanded_args[i][0] != '\0')
        	count++;
    	i++;
	}
    if (count == 0)
    {
        free_args(parsed.args, NULL);
        free_args(expanded_args, NULL);
        free(parsed.quote_types);
        return;
    }
    filtered_args = malloc(sizeof(char *) * (count + 1));
    if (!filtered_args)
    {
        free_args(parsed.args, NULL);
        free_args(expanded_args, NULL);
        free(parsed.quote_types);
        return;
    }
    j = 0;
    i = 0;
	while (expanded_args[i])
	{
    	if (expanded_args[i][0] != '\0')
        	filtered_args[j++] = ft_strdup(expanded_args[i]);
    	i++;
	}
    filtered_args[j] = NULL;
    ft_memset(&data, 0, sizeof(t_command_data));
    parse_input(filtered_args, count, &data, shell);
    free_args(parsed.args, NULL);
    free_args(expanded_args, NULL);
    free_args(filtered_args, NULL);
    free(parsed.quote_types);
    if (validate_command(data.commands, shell) == 0)
        return;
    execute_commands(&data, shell);
}

int	process_input(char *input, t_shell *shell)
{
	int i;

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
	g_signal = 0;
	i = 0;
	while (input[i] && ft_isspace(input[i]))
		i++;
	add_history(input);
	if (is_var_assignment(input))
	{
		handle_var_assignment(input, shell);
		return (1);
	}
	return (2);
}

int main(int argc, char *argv[], char *envp[])
{
	t_shell *shell;
	char *input;

	(void)argc;
	(void)argv;
	shell = get_shell();
	init_shell(shell, envp);
	rl_catch_signals = 0;
	while (1)
	{
		input = readline("minishell> ");
		if (process_input(input, shell) == 0)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break;
		}
		if (input && input[0] != '\0' && process_input(input, shell) == 2)
		{
			handle_command(input, shell);
			if (input && input[0] != '\0')
				free(input);
		}
	}
	finalize_shell(shell);
	return (shell->exit_status);
}
