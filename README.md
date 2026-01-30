
## MINISHELL


## Overview
--------
This repository contains a small educational UNIX-style shell written in C ("minishell").
It implements a number of basic shell features, including:

- Running external commands (via execvp)
- Loading a list of known external commands from `external_cmds.txt`
- Some basic parsing for arguments and simple pipelines
- Builtin (internal) commands: echo, cd, pwd, jobs, fg, bg, exit and a few others
- Basic job control: stop a job with Ctrl-Z (SIGTSTP), resume with `fg` or `bg`, and list stopped jobs with `jobs`
- Signal handling for SIGINT, SIGTSTP, and SIGCHLD (with careful handling in the main loop)

## Files / important sources
-------------------------
- `main.c` — entry point; prints prompt and calls `scan_input()`
- `scan_input.c` — main loop: reads user input, identifies builtin vs external commands, forks for external commands and handles job list updates
- `commands.c` — heavy lifting: parsing input into tokens, executing external commands, reading `external_cmds.txt`
- `handler.c` — signal handlers for SIGINT, SIGTSTP, and SIGCHLD
- `linked_list.c` — small linked list used to track stopped jobs (job list)
- `internal_cmds.c` — table of builtin commands and the implementations (jobs/fg/bg, etc.)
- `header.h` — global declarations, constants and shared structures
- `external_cmds.txt` — list of external commands used by the shell for recognition

## Build
-----
On Linux / WSL you can compile the project with gcc from the project directory:

```sh
gcc -Wall -Wextra -o minishell *.c
```

## Notes:
- The code uses POSIX APIs (fork, execvp, waitpid, signals) — build/run on UNIX-like environments (Linux, WSL, macOS).
- If using a Windows native environment, you will need a POSIX compatibility layer; the project is intended for Unix-like systems.

## Run
---
From the build folder run:

```sh
./minishell
```

Then use the shell like a regular shell. Examples:

- Start jobs and stop with Ctrl-Z:
	- `sleep 10` [Ctrl-Z]
	- `jobs` — lists stopped processes stored in the job list
	- `fg` — resume the most recently stopped job in foreground
	- `bg` — resume the most recently stopped job in background (doesn't wait)

## Known behavior and important implementation notes
-----------------------------------------------
- Signal safety: signal handlers do not call non-async-signal-safe functions. The SIGCHLD handler sets a signal-safe notification flag and the main loop handles reaping and job-list updates. This avoids heap corruption and crashes caused by calling malloc/strcpy/free in signal handlers.
- Job control is implemented in a simple way: the `insert_at_first`, `delete_pid_node`, and `print_stop_process` operations manage jobs in a singly linked list (new stopped jobs are inserted at the head of the list).
- The current implementation:
	- Adds stopped processes to the job list in the main loop when waitpid(Pid, &status, WUNTRACED) observes WIFSTOPPED.
	- When the SIGCHLD notification flag is set, the main loop calls waitpid(-1, WNOHANG) in a loop to reap terminated children and removes them from the job list.

## Known limitations / issues
------------------------
- The parser and buffers use small fixed-size arrays (many places use 20 or 25 bytes). Inputs larger than these fixed sizes can cause incorrect parsing or truncation. Consider switching to dynamic buffers or larger safe limits.
- The pipeline code handles multiple commands but is relatively simple — there are edge cases (e.g. complex quoting, redirection) that are not supported.
- The job control implementation is intentionally small. For full POSIX job control you would need proper process group management (setpgid/ tcsetpgrp ) and advanced handling of SIGTTOU/SIGTTIN.
- SIGCHLD handling here uses a simple boolean notification flag. Under very high child concurrency it may be beneficial to use a self-pipe or a small async-safe ring buffer to keep track of multiple events.


## Contribution and License
----------------------
This is an educational project. You're welcome to open issues or submit PRs with improvements.

## Contact / Notes
---------------
If you want me to implement the self-pipe pattern or to add more robust tests for job control I can implement that next; it will improve stability and correctness when many children change state simultaneously.

## Sample Execution

| Step | Terminal Output |
|------|-----------------|
| **1. Startup & Basic Commands** | <img width="700" alt="minishell0" src="https://github.com/user-attachments/assets/b97acd47-de90-4d1a-bf6c-887fb3ce52e8" /> |
| **2. Customize Hostname** | <img width="700" alt="minishell1" src="https://github.com/user-attachments/assets/19fe357c-0449-46ac-91c7-677aba0ef622" /> |
| **3. Internal Commands** | <img width="700" alt="minishell2" src="https://github.com/user-attachments/assets/4250dfa5-a86b-4bc3-abbb-4bc008c61e8d" /> |
| **4. External Commands** | <img width="700" alt="minishell3" src="https://github.com/user-attachments/assets/fb983497-0113-4af6-8eff-e056ee49c506" /> |
| **5. Job Control & fg/bg to Resume Jobs** | <img width="700" alt="minishell4" src="https://github.com/user-attachments/assets/7adc63b3-9ccf-4750-89bc-9ee65c89031f" /> |

**Interactive shell with working job control, builtins, and signal-safe behavior.**

