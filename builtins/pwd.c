/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:06 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/18 15:44:37 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_pwd(void)
{
	char	cwd[4096];

	if (getcwd(cwd, 4096))
	{
		ft_putendl_fd(cwd, 1);
		return (1);
	}
	else
		return (0);
}
