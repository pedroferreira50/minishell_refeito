/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:40:55 by scarlos-          #+#    #+#             */
/*   Updated: 2025/05/03 11:41:20 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	init_heredoc_pipe(int *pipefd)
{
	int	result;

	result = pipe(pipefd);
	if (result == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

static char	*read_heredoc_line(void)
{
	char	*line;

	line = NULL;
	if (isatty(fileno(stdin)))
	{
		line = readline("> ");
	}
	else
	{
		line = get_next_line(fileno(stdin));
	}
	if (g_signal == SIGINT)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

static void	write_heredoc_line(int pipefd, char *line,
				int suppress_expansion, t_shell *shell)
{
	char	*expanded_line;

	expanded_line = NULL;
	if (suppress_expansion != 0)
	{
		expanded_line = ft_strdup(line);
	}
	else
	{
		expanded_line = expand_variables(line, '\0', shell);
	}
	if (expanded_line != NULL)
	{
		write(pipefd, expanded_line, ft_strlen(expanded_line));
		write(pipefd, "\n", 1);
		free(expanded_line);
	}
}

int	setup_heredoc(const char *delimiter, t_shell *shell, int suppress_expansion)
{
	int		pipefd[2];
	char	*line;

	line = NULL;
	if (delimiter == NULL || init_heredoc_pipe(pipefd) == -1)
		return (-1);
	while (1)
	{
		line = read_heredoc_line();
		if (line == NULL)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			return (-1);
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			close(pipefd[1]);
			return (pipefd[0]);
		}
		write_heredoc_line(pipefd[1], line, suppress_expansion, shell);
		free(line);
	}
}
