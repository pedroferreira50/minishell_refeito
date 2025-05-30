/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:19:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/30 05:38:58 by pviegas-         ###   ########.fr       */
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
	int	i;
	int	j;

	i = 0;
	if (!data)
		return ;
	if (data->commands)
		free_data_commands(data->commands, data->num_commands);
	if (data->arguments)
		free_data_arguments(data->arguments, data->num_commands);
	if (data->input_files)
	{
		while (i < data->num_commands)
		{
			if (data->input_files[i])
				free(data->input_files[i]);
			i++;
		}
		free(data->input_files);
	}
	if (data->heredoc_delim)
		free(data->heredoc_delim);
	if (data->out_redirs && data->num_out_redirs)
	{
		i = 0;
		while (i < data->num_commands)
		{
			if (data->out_redirs[i])
			{
				j = 0;
				while (j < data->num_out_redirs[i])
				{
					if (data->out_redirs[i][j].file)
						free(data->out_redirs[i][j].file);
					j++;
				}
				free(data->out_redirs[i]);
			}
			i++;
		}
		free(data->out_redirs);
		free(data->num_out_redirs);
	}
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
