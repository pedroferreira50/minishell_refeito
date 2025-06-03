/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:05:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 06:43:56 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_assignment_non_export(char *input, t_parse_result *parsed)
{
	(void)input;
	free_args(parsed->args, NULL);
	free(parsed->quote_types);
}

void	handle_command(char *input, t_shell *shell)
{
	t_parse_result	parsed;
	t_command_data	*data;
	char			**expanded_args;
	char			**filtered_args;
	int				i;
	int				j;
	int				count;
	int				pre_parse_exit_status;

	parsed = parse_command(input, shell);
	if (!parsed.args)
		return ;
	expanded_args = expand_tokens(parsed.args, parsed.quote_types, shell);
	if (!expanded_args)
	{
		free_args(parsed.args, NULL);
		free(parsed.quote_types);
		return ;
	}
	count = 0;
	while (expanded_args[count])
		count++;
	if (count == 0)
	{
		free_args(parsed.args, NULL);
		free_args(expanded_args, NULL);
		free(parsed.quote_types);
		return ;
	}
	filtered_args = malloc(sizeof(char *) * (count + 1));
	if (!filtered_args)
	{
		free_args(parsed.args, NULL);
		free_args(expanded_args, NULL);
		free(parsed.quote_types);
		return ;
	}
	i = 0;
	j = 0;
	while (expanded_args[i])
	{
		filtered_args[j] = ft_strdup(expanded_args[i]);
		j++;
		i++;
	}
	filtered_args[j] = NULL;
	data = malloc(sizeof(t_command_data));
	if (!data)
	{
		free_args(parsed.args, NULL);
		free_args(expanded_args, NULL);
		free_args(filtered_args, NULL);
		free(parsed.quote_types);
		return ;
	}
	ft_memset(data, 0, sizeof(t_command_data));
	pre_parse_exit_status = shell->exit_status;
	parse_input(filtered_args, count, data, shell);
	free_args(parsed.args, NULL);
	free_args(expanded_args, NULL);
	free_args(filtered_args, NULL);
	free(parsed.quote_types);
	if (pre_parse_exit_status == 0 && shell->exit_status == 1
		&& data->num_pipes == 0)
	{
		free_command_data(data);
		free(data);
		return ;
	}
	if (shell->exit_status != 2 && validate_command(data->commands, shell) == 0)
	{
		free_command_data(data);
		free(data);
		return ;
	}
	if (shell->exit_status == 2)
	{
		free_command_data(data);
		free(data);
		return ;
	}
	execute_commands(data, shell);
	free_command_data(data);
	free(data);
}

int	process_input(char *input, t_shell *shell)
{
	int	i;

	if (g_signal == SIGINT)
	{
		shell->exit_status = 130;
		g_signal = 0;
		return (1);
	}
	if (input == NULL)
		return (0);
	if (input[0] == '\0')
		return (1);
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

int	main(int argc, char *argv[], char *envp[])
{
	t_shell	*shell;
	char	*input;
	char	*line;
	int		result;

	(void)argc;
	(void)argv;
	shell = get_shell();
	init_shell(shell, envp);
	rl_catch_signals = 0;
	setup_signals();
	while (1)
	{
		if (isatty(fileno(stdin)))
			input = readline("minishell> ");
		else
		{
			line = get_next_line(fileno(stdin));
			if (!line)
			{
				input = NULL;
			}
			else
			{
				input = ft_strtrim(line, "\n");
				free(line);
			}
		}
		
		result = process_input(input, shell);
		if (result == 0)
		{
			break ;
		}
		if (input && input[0] != '\0' && result == 2)
		{
			handle_command(input, shell);
		}
		if (input)
			free(input);
	}
	finalize_shell(shell);
	return (shell->exit_status);
}

/* int	main(int argc, char *argv[], char *envp[])
{
	t_shell	*shell;
	char	*input;

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
			break ;
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
} */

