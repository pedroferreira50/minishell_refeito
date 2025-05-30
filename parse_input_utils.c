#include "minishell.h"

int	is_operator(const char *token)
{
	if (token == NULL)
		return (0);
	if (ft_strcmp(token, "|") == 0)
		return (1);
	if (ft_strcmp(token, "<") == 0)
		return (1);
	if (ft_strcmp(token, ">") == 0)
		return (1);
	if (ft_strcmp(token, ">>") == 0)
		return (1);
	if (ft_strcmp(token, "<<") == 0)
		return (1);
	return (0);
}
