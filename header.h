#ifndef MAIN_H
#define MAIN_H

#include <stdio.h> //headers files
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define BUILTIN 1 // Macros
#define EXTERNAL 2
#define NO_COMMAND 3

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/*Linked list structure*/
typedef struct process
{
    pid_t pid;
    char data[50];
    struct process *next;
} process;

/*FUNCTIONS*/

void scan_input(char *prompt, char *input_string);
void extract_external_commands(char **external_commands);
int check_command_type(char *command, char **external_cmds);
char *get_command(char *input_string);

void execute_internal_commands(char *input_string);
void execute_external_commands(char *input_string);

void signal_handler(int sig_num);

void delete_pid_node(pid_t del_pid);
void delete_at_first(void);
void insert_at_first(pid_t Pid, char *input_string);
void print_stop_process(void);

#endif