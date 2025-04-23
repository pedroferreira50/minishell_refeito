/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:32 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/23 19:14:17 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	ft_set_env(char **envp, const char *name, const char *value)
{
	size_t	name_l;
	size_t	value_len;
	int		i;
	char	*new_entry;

	name_l = ft_strlen(name);
	value_len = ft_strlen(value);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, name_l) == 0 && envp[i][name_l] == '=')
		{
			new_entry = malloc(name_l + value_len + 2);
			if (!new_entry)
				return (-1);
			ft_strcpy(new_entry, name);
			new_entry[name_l] = '=';
			ft_strcpy(new_entry + name_l + 1, value);
			free(envp[i]);
			envp[i] = new_entry;
			return (0);
		}
		i++;
	}
	return (-1);
}

static int	gotohome(char **path, t_shell *shell)
{
	*path = getenv("HOME");
	if (*path == NULL)
	{
		perror("cd: HOME not set\n");
		shell->exit_status = 1;
		return (1);
	}
	return (0);
}

static int	handle_cd_error(t_shell *shell, char *old_pwd, const char *error_msg)
{
	if (error_msg)
		perror(error_msg);
	shell->exit_status = 1;
	free(old_pwd);
	return (1);
}

int	ft_cd(char **args, int *i, t_shell *shell)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;

	old_pwd = getcwd(NULL, 0);
	if (args[1] == NULL || strcmp(args[1], "~") == 0)
		gotohome(&path, shell);
	else
		path = args[1];
	if (chdir(path) != 0)
		return (handle_cd_error(shell, old_pwd, "cd"));
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (handle_cd_error(shell, old_pwd, "getcwd"));
	if (old_pwd)
		ft_set_env(shell->envp, "OLDPWD", old_pwd);
	ft_set_env(shell->envp, "PWD", new_pwd);
	free(old_pwd);
	free(new_pwd);
	if(i)
		(*i)++;
	shell->exit_status = 0;
	return (1);
}
