#include "header.h"

extern char *external_cmds[];// call global values using extern
extern char *builtins[];
extern int status;
extern process *head;

void extract_external_commands(char **external_commands) // fn for external commands extraction from txt file
{
  int fd = open("external_cmds.txt", O_RDONLY);
  if (fd == -1)
  {
    if (errno == EACCES)
      perror("open");
  }

  char ch, buff[20];
  int i = 0, j = 0;
  while (read(fd, &ch, 1) > 0)
  {
    if (ch != 13) // The ASCII value (13) is a Carriage Return (CR) control character(vs-code error fix)
    {
      buff[i++] = ch;
    }

    if (ch == '\n')
    {
      buff[i - 1] = '\0';
      external_commands[j] = malloc((strlen(buff) + 1) * sizeof(char)); // allocate memory
      strcpy(external_commands[j], buff);                               // copy the commands to 2d array

      j++;
      i = 0;
    }
  }

  if (i > 0)
  {
    buff[i] = '\0';
    external_commands[j] = malloc((strlen(buff) + 1) * sizeof(char));
    strcpy(external_commands[j], buff);
  }

  close(fd);
  return;
}

char *get_command(char *input_string) // fn to get command from input string
{
  char *cmd = malloc(20 * sizeof(char));
  if (!cmd)
  {
    perror("MALLOC");
    return NULL;
  }

  for (int i = 0; i < 20; i++)
  {
    cmd[i] = input_string[i];
    if (cmd[i] == ' ')
    {
      cmd[i] = '\0';
      char *temp = realloc(cmd, i + 1);
      if (!temp)
      {
        perror("REALLOC");
        return NULL;
      }

      cmd = temp;
      return cmd;
    }
  }

  cmd[19] = '\0';
  return cmd;
}

int check_command_type(char *command, char **external_cmds) // fn to check command type
{
  for (int i = 0; builtins[i] != NULL; i++)
  {
    if (!strcmp(builtins[i], command))
      return BUILTIN;
  }

  for (int i = 0; i < 152; i++)
  {
    if (!strcmp(external_cmds[i], command))
      return EXTERNAL;
  }

  return NO_COMMAND;
}

void execute_internal_commands(char *input_string) // fn to execute internal commands
{
  if (!strncmp(input_string, "exit", 4))
  {
    for (int i = 0; i < 152; i++)
    {
      free(external_cmds[i]);
    }

    exit(0);
  }
  else if (!strncmp(input_string, "pwd", 3))
  {
    char buff[100];
    getcwd(buff, 100);
    printf("%s\n", buff);
  }
  else if (!strncmp(input_string, "cd", 2))
  {
    chdir(input_string + 3);
    char buff[100];
    getcwd(buff, 100);
    printf("%s\n", buff);
  }
  else if (!strncmp(input_string, "echo $$", 7))
  {
    printf("%d\n", getpid()); // print the current process id
  }
  else if (!strncmp(input_string, "echo $?", 7))
  {
    printf("%d\n", WEXITSTATUS(status)); // print prev execution is success or not
  }
  else if (!strncmp(input_string, "echo $SHELL", 11))
  {
    char *buff = getenv("SHELL");
    if(buff != NULL)
       printf("%s\n", buff);
  }
  else if (!strcmp(input_string, "jobs"))
  {
    print_stop_process(); // print the stopped process list
  }
  else if (!strcmp(input_string, "fg"))
  {
    if (head == NULL)
    {
      printf("-bash: fg: current: no such job\n");
    }
    else
    {
      kill(head->pid, SIGCONT);
      waitpid(head->pid, &status, WUNTRACED);
      delete_at_first();
    }
  }
  else if (!strcmp(input_string, "bg"))
  {
    if (head == NULL)
    {
      printf("-bash: bg: current: no such job\n");
    }
    else
    {
      kill(head->pid, SIGCONT);
    }
  }

  return;
}

void execute_external_commands(char *input_string) // fn to execute external commands
{
  char *str = input_string;
  int rows = 0, i = 0;
  while (str[i])
  {
    if ((str[i] == ' ' && i > 0 && str[i - 1] != ' ') || (str[i + 1] == '\0' && str[i] != ' '))
      rows++;
    i++;
  }
  // printf("count-->%d\n", rows); // test

  char *input_cmds[rows + 1];
  input_cmds[rows] = NULL;

  int j = 0, k = 0, pipes = 0;
  str = input_string, i = 0;
  char buff[20];

  int *index = (int *)malloc(20 * sizeof(int)); // allocate memory
  if (index == NULL)
  {
    perror("MALLOC");
    return;
  }
  index[0] = 0;

  while (str[i])
  {
    if (str[i] != ' ')
      buff[j++] = str[i];
    else if (str[i] == ' ' && i > 0 && str[i - 1] != ' ') // Only add single space between commands
      buff[j++] = str[i];

    if ((str[i] == ' ' && i > 0 && str[i - 1] != ' ') || (str[i + 1] == '\0' && str[i] != ' '))
    {
      if (str[i + 1] == '\0')
        buff[j] = '\0';
      else
        buff[j - 1] = '\0';

      input_cmds[k] = malloc((strlen(buff) + 1) * sizeof(char));

      if (!strcmp(buff, "|"))
      {
        index[++pipes] = k + 1;
        input_cmds[k] = NULL;
      }
      else
        strcpy(input_cmds[k], buff);

      k++;
      j = 0;
    }
    i++;
  }

  if (pipes == 0)
  {
    free(index);
  }
  else
  {
    int *temp = (int *)realloc(index, (pipes + 1) * sizeof(int)); // reallocate memory
    if (temp == NULL)
    {
      perror("REALLOC FAILED");
      free(index);
      return;
    }
    index = temp;
  }

  // i = 0;
  // while (i < rows) // test
  // {
  //   if (input_cmds[i] != NULL)
  //     printf("index0[%d]-->%s-->size:%ld\n", i, input_cmds[i], strlen(input_cmds[i]));
  //   i++;
  // }

  // i = 0; // test
  // printf("pipes : %d\n", pipes);
  // while (i <= pipes && pipes != 0)
  // {
  //   // if(input_cmds[i] != NULL)
  //   printf("index[%d]-->%s-->size:%ld\n", i, input_cmds[index[i]], strlen(input_cmds[index[i]]));
  //   i++;
  // }

  if (pipes == 0) // if there is no pipe
  {
    // printf("------>%s\n", input_cmds[0]);
    execvp(input_cmds[0], input_cmds);
    perror(input_cmds[0]); // Print error if command not found
    exit(0);
  }
  else
  {
    int pipe_fd[2];

    for (int i = 0; i <= pipes; i++)
    {
      if (i != pipes)
      {
        if (pipe(pipe_fd) == -1) // creating pipe
        {
          perror("PIPE");
          free(index);
          return;
        }
      }

      pid_t child = fork();

      if (child > 0) // parent
      {
        if (i != pipes)
        {
          dup2(pipe_fd[0], 0);
          close(pipe_fd[0]);
          close(pipe_fd[1]);
        }

        wait(NULL);
      }
      else if (child == 0) // child
      {
        if (i != pipes)
        {
          dup2(pipe_fd[1], 1);
          close(pipe_fd[0]);
        }

        execvp(input_cmds[index[i]], input_cmds + index[i]);
        perror(input_cmds[index[i]]); // Print error if command not found
        exit(0);
      }
    }
  }

  free(index);
  for (int i = 0; i < rows; i++)
  {
    free(input_cmds[i]);
  }
}

