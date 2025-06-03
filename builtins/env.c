/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:07 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/03 07:52:37 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	free_env_copy(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		free(env[i++]);
	free(env);
}

char	**copy_envp(char **envp)
{
	int		i;
	char	**new_envp;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc((i + 1) * sizeof(char *));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (free_env_copy(new_envp), NULL);
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}

/* static void	sort_env(char **env, int count)
{
	char	*tmp;
	int		i;
	int		j;

	i = -1;
	while (++i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
	}
} */

static void	print_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
		printf("%s\n", env[i++]);
}

void	ft_env(char **args, t_shell *shell)
{
	char	**copy;
	int		count;

	if (args && args[1])
	{
		print_error_command("env", "No such file or directory", 127, shell);
		return;
	}

	count = 0;
	while (shell->envp[count])
		count++;
	copy = copy_envp(shell->envp);
	if (!copy)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	print_env(copy);
	free_env_copy(copy);
	shell->exit_status = 0;
}
