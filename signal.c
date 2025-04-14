#include "minishell.h"

int g_signal = 0;

void sigint_handler(int signum)
{
    g_signal = signum;
    rl_replace_line("", 0);
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_redisplay();
}

void setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        write(STDERR_FILENO, "sigaction for SIGINT\n", 21);
        exit(1);
    }
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
    {
        write(STDERR_FILENO, "sigaction for SIGQUIT\n", 22);
        exit(1);
    }
}