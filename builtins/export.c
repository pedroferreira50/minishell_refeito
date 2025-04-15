/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/15 13:11:38 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int handle_invalid_identifier(char *name, t_shell *shell)
{
    ft_putstr_fd("minishell: export: `", 2);
    ft_putstr_fd(name, 2);
    ft_putstr_fd("': not a valid identifier\n", 2);
    shell->exit_status = 1;
    return (1);
}

static void update_env_var(char *name, char *value, t_shell *shell)
{
    char *temp;
    char *new_entry;
    int index;

    temp = ft_strjoin(name, "=");
    new_entry = ft_strjoin(temp, value);
    free(temp);
    if (!shell->envp)
    {
        shell->envp = malloc(sizeof(char *) * 2);
        if (!shell->envp)
        {
            free(new_entry);
            return;
        }
        shell->envp[0] = ft_strdup(new_entry);
        shell->envp[1] = NULL;
        free(new_entry);
        return;
    }
    index = find_env_var_index(shell, name);
    shell->envp = copy_envp_with_update(shell, new_entry, index);
    free(new_entry);
}

int handle_export_with_value(char *arg, t_shell *shell)
{
    char *equal;
    char *name;
    char *value;
    t_var *var;

    equal = ft_strchr(arg, '=');
    if (!equal)
        return (1);
    *equal = '\0';
    name = arg;
    value = equal + 1;
    if (!is_valid_var_name(name))
        return (handle_invalid_identifier(name, shell));
    store_var(name, value, &shell->vars);
    var = shell->vars;
    while (var && ft_strcmp(var->name, name) != 0)
        var = var->next;
    if (var)
    {
        var->exported = 1;
        update_env_var(name, value, shell);
    }
    return (0);
}

int handle_export_without_value(char *arg, t_shell *shell)
{
    t_var *var;

    if (!is_valid_var_name(arg))
        return (handle_invalid_identifier(arg, shell));
    var = shell->vars;
    while (var && ft_strcmp(var->name, arg) != 0)
        var = var->next;
    if (var)
    {
        var->exported = 1;
        update_env_var(var->name, var->value, shell);
    }
    else
    {
        ft_putstr_fd("minishell: export: `", 2);
        ft_putstr_fd(arg, 2);
        ft_putstr_fd("': not found\n", 2);
        shell->exit_status = 1;
        return (1);
    }
    return (0);
}

int ft_export(char **args, t_shell *shell)
{
    int i;
    int status;
    char *equal;

    status = 0;
    if (!args[1])
        return (print_exported_env(shell));
    i = 1;
    while (args[i])
    {
        equal = ft_strchr(args[i], '=');
        if (equal)
            status |= handle_export_with_value(args[i], shell);
        else
            status |= handle_export_without_value(args[i], shell);
        i++;
    }
    if (status != 0)
        shell->exit_status = 1;
    return (status);
}
