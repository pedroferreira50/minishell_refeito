/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:19:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/03 11:22:25 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_data_commands(char **commands, int num_commands)
{
	int	i;

	if (commands)
	{
		i = 0;
		while (i < num_commands)
		{
			if (commands[i])
				free(commands[i]);
			i++;
		}
		free(commands);
	}
}

void	free_data_arguments(char ***arguments, int num_commands)
{
	int	i;
	int	j;

	if (!arguments)
		return ;
	i = 0;
	while (i < num_commands)
	{
		if (arguments[i])
		{
			j = 0;
			while (arguments[i][j])
			{
				free(arguments[i][j]);
				j++;
			}
			free(arguments[i]);
		}
		i++;
	}
	free(arguments);
}

void	free_command_data(t_command_data *data)
{
	if (!data)
		return ;
	if (data->commands)
		free_data_commands(data->commands, data->num_commands);
	if (data->arguments)
		free_data_arguments(data->arguments, data->num_commands);
	if (data->input_file)
		free(data->input_file);
	if (data->output_file)
		free(data->output_file);
	if (data->heredoc_delim)
		free(data->heredoc_delim);
	data->num_commands = 0;
	data->num_pipes = 0;
}

void	free_args(char **args, t_command_data *data)
{
	int	i;

	i = 0;
	while (args && args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
	if (data)
		free_command_data(data);
}
