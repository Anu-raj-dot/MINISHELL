#include "header.h"

extern pid_t Pid;
extern char prompt[25];
extern char input_string[25];
extern int status;
extern volatile sig_atomic_t minishell_sigchld;

void signal_handler(int sig_num)
{
    if (sig_num == SIGINT)
    {
        if (Pid == 0)
        {
            printf(ANSI_COLOR_RED"\n%s"ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }
    }

    else if (sig_num == SIGTSTP)
    {
        if (Pid == 0)
        {
            printf(ANSI_COLOR_GREEN"\n%s"ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }

    }

    else if (sig_num == SIGCHLD)
    {
        minishell_sigchld = 1;
    }
}