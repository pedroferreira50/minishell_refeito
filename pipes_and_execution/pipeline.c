#include "../minishell.h"

static void setup_input_redirect(t_command_data *data, int i, t_shell *shell)
{
    int fd;

    fd = -1;
    if (i == 0 && data->input_file != NULL)
    {
        fd = open(data->input_file, O_RDONLY);
        if (fd < 0)
        {
            perror("open input file");
            shell->exit_status = 1;
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
}

static void setup_output_redirect(t_command_data *data, int i, int num_commands, t_shell *shell)
{
    int fd;
    int flags;

    fd = -1;
    flags = O_WRONLY | O_CREAT;
    if (i == num_commands - 1 && data->output_file != NULL)
    {
        if (data->append_output != 0)
            flags = flags | O_APPEND;
        else
            flags = flags | O_TRUNC;
        fd = open(data->output_file, flags, 0644);
        if (fd < 0)
        {
            perror("open output file");
            shell->exit_status = 1;
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

void setup_pipes_and_redirections(t_command_data *data, t_exec_state *state, int num_commands, t_shell *shell)
{
    if (state->i > 0)
    {
        dup2(state->prev_pipe_read, STDIN_FILENO);
        close(state->prev_pipe_read);
    }
    setup_input_redirect(data, state->i, shell);
    if (state->i < num_commands - 1)
    {
        dup2(state->pipefd[1], STDOUT_FILENO);
    }
    setup_output_redirect(data, state->i, num_commands, shell);
}

int setup_pipeline(t_command_data *data, t_exec_state *state, t_shell *shell)
{
    int result;

    result = 0;
    if (state->i < data->num_commands - 1)
    {
        result = pipe(state->pipefd);
        if (result == -1)
        {
            perror("minishell: pipe");
            shell->exit_status = 1;
            return (-1);
        }
    }
    if (data->heredoc_delim != NULL && state->i == 0)
    {
        state->heredoc_fd = setup_heredoc(data->heredoc_delim, shell, data->heredoc_quoted);
        if (state->heredoc_fd == -1)
        {
            shell->exit_status = 1;
            return (-1);
        }
    }
    return (0);
}
