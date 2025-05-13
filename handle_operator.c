#include "minishell.h"

void	handle_pipe(t_command_data *data, int *command_index, t_shell *shell)
{
	data->num_pipes++;
	(*command_index)++;
	shell->exit_status = 0;
}

void handle_redirect(char **args, t_command_data *data, t_indices *indices, t_shell *shell)
{
    const char *token = args[indices->i];
    if (ft_strcmp(token, "<") == 0 && args[indices->i + 1] != NULL)
    {
        free(data->input_file);
        data->input_file = ft_strdup(args[indices->i + 1]);
        if (data->input_file == NULL)
            shell->exit_status = 1;
        indices->i += 2;
    }
    else if (ft_strcmp(token, ">") == 0 && args[indices->i + 1] != NULL)
    {
        free(data->output_file);
        data->output_file = ft_strdup(args[indices->i + 1]);
        if (data->output_file == NULL)
            shell->exit_status = 1;
        data->append_output = 0;
        indices->i += 2;
    }
    else if (ft_strcmp(token, ">>") == 0 && args[indices->i + 1] != NULL)
    {
        free(data->output_file);
        data->output_file = ft_strdup(args[indices->i + 1]);
        if (data->output_file == NULL)
            shell->exit_status = 1;
        data->append_output = 1;
        indices->i += 2;
    }
    else
    {
        ft_putstr_fd("minishell: syntax error near redirect\n", 2);
        shell->exit_status = 2;
        indices->i++;
    }
}

void handle_heredoc(char **args, t_command_data *data, t_indices *indices, t_shell *shell)
{
    size_t len;
    char *delim;

    if (args[indices->i + 1] != NULL)
    {
        free(data->heredoc_delim);
        delim = args[indices->i + 1];
        len = ft_strlen(delim);
        if (len >= 2 && (delim[0] == '"' || delim[0] == '\'') && delim[len - 1] == delim[0])
        {
            data->heredoc_delim = ft_strndup(delim + 1, len - 2);
            data->heredoc_quoted = 1;
        }
        else
        {
            data->heredoc_delim = ft_strdup(delim);
            data->heredoc_quoted = 0;
        }
        if (data->heredoc_delim == NULL)
            shell->exit_status = 1;
        indices->i += 2;
    }
    else
    {
        ft_putstr_fd("minishell: syntax error: no delimiter after '<<'\n", 2);
        shell->exit_status = 2;
        indices->i++;
    }
}
