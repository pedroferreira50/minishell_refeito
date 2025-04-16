#include "../minishell.h"

void execute_command(char *command, char **args, t_shell *shell)
{
    char *full_path;

    full_path = NULL;
    full_path = find_command_path(command, shell);
    if (full_path == NULL)
    {
        ft_putstr_fd("Command not found: ", 2);
        ft_putstr_fd(command, 2);
        ft_putstr_fd("\n", 2);
        shell->exit_status = 127;
        g_signal = 127;
        exit(127);
    }
    execve(full_path, args, shell->envp);
    perror("execve");
    free(full_path);
    exit(shell->exit_status);
}

static pid_t *init_execution(t_command_data *data, t_exec_state *state, t_shell *shell)
{
    pid_t *pids;

    pids = NULL;
    if (data == NULL || data->commands == NULL || data->num_commands == 0)
    {
        shell->exit_status = 2;
        return (NULL);
    }
    pids = malloc(sizeof(pid_t) * data->num_commands);
    if (pids == NULL)
    {
        shell->exit_status = 1;
        return (NULL);
    }
    ft_memset(pids, 0, sizeof(pid_t) * data->num_commands);
    state->prev_pipe_read = -1;
    state->heredoc_fd = -1;
    state->i = 0;
    return (pids);
}

static void fork_child(t_command_data *data, t_exec_state *state, t_shell *shell)
{
    int has_builtin;

    has_builtin = 0;
    if (data->commands[state->i] == NULL)
        exit(0);
    if (data->commands[state->i] != NULL)
        has_builtin = check_builtin(data->commands[state->i]);
    if (state->heredoc_fd != -1)
    {
        dup2(state->heredoc_fd, STDIN_FILENO);
        close(state->heredoc_fd);
    }
    setup_pipes_and_redirections(data, state, data->num_commands, shell);
    if (state->i < data->num_commands - 1)
    {
        close(state->pipefd[0]);
    }
    if (has_builtin != 0)
    {
        execute_builtin(&state->i, shell, data);
        exit(shell->exit_status);
    }
    execute_command(data->commands[state->i], data->arguments[state->i], shell);
    exit(1);
}

static void manage_parent(pid_t pid, pid_t *pids, t_exec_state *state, t_command_data *data)
{
    if (pid > 0)
    {
        pids[state->i] = pid;
        if (state->prev_pipe_read != -1)
        {
            close(state->prev_pipe_read);
            state->prev_pipe_read = -1;
        }
        if (state->i < data->num_commands - 1)
        {
            state->prev_pipe_read = state->pipefd[0];
            close(state->pipefd[1]);
        }
        if (state->heredoc_fd != -1)
        {
            close(state->heredoc_fd);
            state->heredoc_fd = -1;
        }
    }
}

static void run_pipeline(t_command_data *data, t_exec_state *state, t_shell *shell, pid_t *pids)
{
    pid_t pid;

    if (setup_pipeline(data, state, shell) == -1)
    {
        free(pids);
        shell->exit_status = 1;
        return;
    }
    pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        fork_child(data, state, shell);
    }
    else
    {
        if (pid < 0)
        {
            perror("minishell: fork");
            free(pids);
            shell->exit_status = 1;
            return;
        }
        manage_parent(pid, pids, state, data);
        state->i = state->i + 1;
    }
}

static void handle_wait_status(int status, t_shell *shell)
{
    if (WIFEXITED(status))
    {
        shell->exit_status = WEXITSTATUS(status);
    }
    else
    {
        if (WIFSIGNALED(status))
        {
            shell->exit_status = 128 + WTERMSIG(status);
            if (shell->exit_status == 130)
                ft_putstr_fd("\n", 2);
            if (shell->exit_status == 139)
                ft_putstr_fd("minishell: segmentation fault\n", 2);
        }
    }
}

static void wait_commands(pid_t *pids, t_command_data *data, t_shell *shell)
{
    int i;
    int status;

    i = 0;
    while (i < data->num_commands)
    {
        if (pids[i] > 0)
        {
            if (waitpid(pids[i], &status, 0) > 0)
                handle_wait_status(status, shell);
        }
        i = i + 1;
    }
    free(pids);
}

void execute_commands(t_command_data *data, t_shell *shell)
{
    t_exec_state state;
    pid_t *pids;
    int i;

    state.pipefd[0] = 0;
    state.pipefd[1] = 0;
    state.prev_pipe_read = -1;
    state.heredoc_fd = -1;
    state.i = 0;
    pids = NULL;
    if (data == NULL || data->commands == NULL || data->num_commands == 0)
    {
        shell->exit_status = 2;
        return;
    }
    pids = init_execution(data, &state, shell);
    if (pids == NULL)
        return;
    while (state.i < data->num_commands)
    {
        run_pipeline(data, &state, shell, pids);
        if (g_signal != 0)
        {
            i = 0;
            while (i < data->num_commands)
            {
                if (pids[i] > 0)
                    kill(pids[i], SIGINT);
                i++;
            }
            if (state.prev_pipe_read != -1)
            {
                close(state.prev_pipe_read);
                state.prev_pipe_read = -1;
            }
            break;
        }
    }
    wait_commands(pids, data, shell);
    free_command_data(data);
    if (state.prev_pipe_read != -1)
    {
        close(state.prev_pipe_read);
        state.prev_pipe_read = -1;
    }
}
