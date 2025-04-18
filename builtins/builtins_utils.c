/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 17:20:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/18 20:21:23 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_builtin(char *command)
{
	if (command == NULL)
		return (0);
	if (ft_strcmp(command, "echo") == 0 || \
		ft_strcmp(command, "pwd") == 0 || \
		ft_strcmp(command, "cd") == 0 || \
		ft_strcmp(command, "export") == 0 || \
		ft_strcmp(command, "unset") == 0 || \
		ft_strcmp(command, "exit") == 0 || \
		ft_strcmp(command, "env") == 0)
		return (1);
	return (0);
}

const char	*get_var_value(const char *name, t_var *vars)
{
	t_var	*var;

	var = vars;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
			return (var->value);
		var = var->next;
	}
	return (NULL);
}

int ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || \
		 c == '\f' || c == '\r');
}
void	parent_builtin(t_command_data *data, t_exec_state *state, t_shell *shell)
{
	char	*cmd;
	char	**args;

	cmd = data->commands[state->i];
	args = data->arguments[state->i];
	if (!data || !data->commands[state->i])
		return ;
	if (ft_strcmp(cmd, "cd") == 0)
		ft_cd(args, &state->i, shell);
	else if (ft_strcmp(cmd, "export") == 0)
		shell->exit_status = ft_export(args, shell);
	else if (ft_strcmp(cmd, "unset") == 0)
		shell->exit_status = ft_unset(args, &shell->vars, &shell->envp);
	else if (ft_strcmp(cmd, "exit") == 0)
		ft_exit(args, shell);
}
