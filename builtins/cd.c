/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:32 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:53:23 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
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
*/

static char	*get_env_value(char **envp, const char *name)
{
	size_t	name_len;
	int		i;

	if (!envp || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, name_len) == 0 && envp[i][name_len] == '=')
			return (envp[i] + name_len + 1);
		i++;
	}
	return (NULL);
}

static int	gotohome(char **path, t_shell *shell)
{
	*path = get_env_value(shell->envp, "HOME");
	if (*path == NULL)
	{
		print_error_simple("cd: HOME not set", 1, shell);
		return (1);
	}
	return (0);
}

static int	goto_oldpwd(char **path, t_shell *shell)
{
	*path = get_env_value(shell->envp, "OLDPWD");
	if (*path == NULL)
	{
		print_error_simple("cd: OLDPWD not set", 1, shell);
		return (1);
	}
	printf("%s\n", *path);
	return (0);
}

static int	handle_cd_error(t_shell *shell, char *old_pwd, char *path)
{
	print_error_command("cd", path, 1, shell);
	ft_putstr_fd(": No such file or directory\n", 2);
	shell->exit_status = 1;
	free(old_pwd);
	return (1);
}

static int	handle_cd_too_many_arguments(t_shell *shell)
{
	print_error_simple("cd: too many arguments", 1, shell);
	return (1);
}


int	ft_cd(char **args, int *i, t_shell *shell)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;
	int		arg_count;

	arg_count = count_args(args);
	if (arg_count > 2)
		return (handle_cd_too_many_arguments(shell));
	old_pwd = getcwd(NULL, 0);
	if (args[1] == NULL || strcmp(args[1], "~") == 0)
	{
		if (gotohome(&path, shell) != 0)
			return (1);
	}
	else if (strcmp(args[1], "-") == 0)
	{
		if (goto_oldpwd(&path, shell) != 0)
			return (1);
	}
	else
		path = args[1];
	if (chdir(path) != 0)
		return (handle_cd_error(shell, old_pwd, path));
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		print_error_simple("cd: getcwd failed", 1, shell);
		free(old_pwd);
		return (1);
	}
	if (old_pwd)
		export_var("OLDPWD", old_pwd, shell);
	export_var("PWD", new_pwd, shell);
	free(old_pwd);
	free(new_pwd);
	if (i)
		(*i)++;
	shell->exit_status = 0;
	return (1);
}
