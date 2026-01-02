#include "header.h"

char *external_cmds[152];
pid_t Pid;
int status;
process *head = NULL;
volatile sig_atomic_t minishell_sigchld = 0;

void scan_input(char *prompt, char *input_string)
{
        extract_external_commands(external_cmds);
        
        // int i =0;                                                //test
        // while(external_cmds[i])
        // {
        //         printf("%s\n",external_cmds[i]);
        //         i++;
        // }

        signal(SIGINT, signal_handler);
        signal(SIGTSTP, signal_handler);
        signal(SIGCHLD, signal_handler);

        while (1)
        {
                Pid = 0;
                printf(ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_RESET, prompt); // printing prompt
                scanf("%[^\n]", input_string);
                char ch;
                while ((ch = getchar()) != '\n'); // clear the input buffer

                if(!strncmp(input_string, "PS1=", 4)) // change prompt name using enviroment variable
                {
                        if (input_string[4] != ' ')
                        {
                                strcpy(prompt, input_string + 4);
                        }
                }
                else
                {
                        char *cmd = get_command(input_string); // to get command from input string
                        //printf("cmd --> %s   ", cmd);          // test
                        //fflush(stdout);
                        int type = check_command_type(cmd, external_cmds); // checking command type

                        if (type == BUILTIN)
                        {
                                //printf("type --> BUILTIN\n");            // test
                                execute_internal_commands(input_string); // call int_execute fn
                        }
                        else if (type == EXTERNAL)
                        {
                                //printf("type --> EXTERNAL\n"); // test

                                Pid = fork(); // creating child process

                                if (Pid > 0)
                                {
                                        waitpid(Pid, &status, WUNTRACED); // return even child process is stopped

                                        /* If this particular child was stopped by SIGTSTP,
                                           add it to the job list here (safe to call malloc/strcpy). */
                                        if (WIFSTOPPED(status))
                                        {
                                                insert_at_first(Pid, input_string);
                                        }
                                }
                                else if (Pid == 0)
                                {
                                        signal(SIGINT, SIG_DFL); // make it default again
                                        signal(SIGTSTP, SIG_DFL);
                                        execute_external_commands(input_string); // call ext_execute fn
                                }
                        }
                        else if (type == NO_COMMAND)
                                printf(ANSI_COLOR_CYAN"cmd --> %s   type --> NO_COMMAND\n"ANSI_COLOR_RESET,cmd); // test

                        /* If SIGCHLD was observed, reap all terminated children and
                           remove them from the job list in the safe context. */
                        if (minishell_sigchld)
                        {
                                pid_t del_pid;
                                while ((del_pid = waitpid(-1, &status, WNOHANG)) > 0)
                                {
                                        if (WIFEXITED(status) || WIFSIGNALED(status))
                                        {
                                                delete_pid_node(del_pid);
                                        }
                                }
                                minishell_sigchld = 0;
                        }

                        free(cmd);
                }
        }
}