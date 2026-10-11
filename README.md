# MiniShell

A lightweight Unix shell implementation written in C, featuring pipes, I/O redirection, background jobs, and command chaining.

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)]()

## Overview

MiniShell reproduces the core behavior of a Unix shell. It parses and executes command lines with support for:

- **Pipes** (single and chained): `ls -la | grep .c | wc -l`
- **Background execution** (`&`): `sleep 10 &`
- **Stdout redirection** (`>`): `echo "hello" > output.txt`
- **Command chaining** (`;`): `mkdir test ; cd test ; pwd`
- **Built-in commands**: `cd`, `pwd`, `exit`, `jobs`
- **Signal handling**: proper `SIGCHLD` management for background processes

## Building

**Prerequisites**: GCC, Make, Linux

```bash
# Build
make

# Run
./bin/minishell

# Clean object files
make clean

# Full clean (objects + binary)
make aclean

# Rebuild from scratch
make rebuild
```

## Usage Examples

```bash
# Simple commands
ls -la
echo "Hello, World!"

# Pipes
cat /etc/hosts | grep localhost
ls -la | grep .c | sort | head -5

# Background execution
sleep 30 &
jobs                    # List background jobs

# Output redirection
ps aux > processes.txt

# Command chaining
ls ; echo "---" ; pwd
compile.sh & echo "Building in background..."
```

## Architecture

```
minishell/
├── src/
│   ├── main.c          # Entry point — main REPL loop
│   ├── shell.c         # Parser and command execution engine
│   ├── command.c       # Command data structure and background job management
│   ├── terminal.c      # User input handling and prompt display
│   └── utils.c         # String utility functions
├── inc/
│   ├── shell.h
│   ├── command.h
│   ├── terminal.h
│   └── utils.h
├── makefile
├── LICENSE
└── README.md
```

## How It Works

The shell runs a Read-Eval-Print Loop (REPL) in `main.c`:

```
┌─────────────────────────────────────────────────┐
│  1. Display prompt (user@host:~/path$)          │
│  2. Read user input                             │
│  3. Validate syntax (reject invalid sequences)  │
│  4. Report completed background jobs            │
│  5. Parse input into Command structures         │
│  6. Execute each command                        │
│  └─→ Loop back to 1                             │
└─────────────────────────────────────────────────┘
```

### Parsing Strategy

The parser performs **three-level tokenization**:

1. **Command level** — Split by `;` and `&` (marks background flag)
2. **Pipe level** — Split by `|` and `>` (marks file redirection)
3. **Argument level** — Split by whitespace

Each command is represented by a `Command` struct:

```c
typedef struct Command {
    char***      args;            // Arguments for each sub-command
    int          n_pipe;          // Number of pipe segments
    int*         n_args;          // Argument count per segment
    int          is_file_output;  // Stdout redirected to file
    int          is_background;   // Run in background
    pid_t        pid;             // Associated process ID
    int          command_id;      // Job ID (displayed in brackets)
    CommandState state;           // RUNNING, DONE, EXIT_126, EXIT_127
    struct Command *next;         // Linked list pointer (for background jobs)
} Command;
```

### Execution

- **Built-in commands** are executed directly in the shell process (no fork)
- **External commands** use `fork()` + `execvp()`
- **Pipes** are implemented using `pipe()` and `dup2()` to chain stdin/stdout between processes
- **Background jobs** are tracked in a global linked list and monitored via `SIGCHLD`

## Limitations

- No support for `stderr` redirection (`2>`) or append mode (`>>`)
- No wildcard/glob expansion (`*`, `?`)
- No environment variable expansion (`$VAR`)
- No quotes handling for arguments with spaces
- Single-line input only (no multi-line or heredoc support)

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

## Author

Jules Guignard
