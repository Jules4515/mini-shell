/**
 * @file shell.c
 * @author Jules Guignard
 * @brief Source file, handles the execution of commands
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "shell.h"
#include <limits.h>

Command** parser(char *str_command, int *n_command)
{
    // Count the number of commands 

    char* token;
    char* str_command_temp = strdup2(str_command);
    token = strtok(str_command_temp, ";&");
    *n_command = 0;
    while(token)
    {
        (*n_command)++;
        token = strtok(NULL, ";&");
    }
    free(str_command_temp);

    // Initialization of the command array

    int l = 0;
    Command** commands = malloc(*n_command * sizeof(Command*));
    for(int i = 0; i < *n_command; i++)
    {
        commands[i] = malloc(sizeof(Command));
        commands[i]->n_pipe = 0;
        while(str_command[l] != ';' && str_command[l] != '&' && str_command[l] != '\0') l++;
        commands[i]->is_background = str_command[l] == '&';
        l++;
    }

    // Tokenization of commands

    char* saveptr1;
    char* token1 = strtok_r(str_command, ";&", &saveptr1);
    int i = 0;

    while(token1)
    {
        // Check if there is an output redirection to a file

        commands[i]->is_file_output = 0;
        for(int j = 0; token1[j] != '\0'; j++)
        {
            if(token1[j] == '>') commands[i]->is_file_output = 1;
        }

        // Count the number of pipes per command

        char* token1_temp = strdup2(token1);
        token = strtok(token1_temp, "|>");
        while(token)
        {
            commands[i]->n_pipe++;
            token = strtok(NULL, "|>");
        }
        free(token1_temp);

        commands[i]->args = malloc(commands[i]->n_pipe * sizeof(char**));
        commands[i]->n_args = malloc(commands[i]->n_pipe * sizeof(int));
        for(int j = 0; j < commands[i]->n_pipe; j++)
        {
            commands[i]->n_args[j] = 0;
        }

        // Tokenization of subcommands (between pipes)

        char* saveptr2;
        char* token2 = strtok_r(token1, "|>", &saveptr2);
        int j = 0;

        while(token2)
        {
            // Count the number of arguments of subcommands

            char* token2_temp = strdup2(token2);
            token = strtok(token2_temp, " ");
            while(token)
            {
                commands[i]->n_args[j]++;
                token = strtok(NULL, " ");
            }
            free(token2_temp);

            commands[i]->args[j] = malloc((commands[i]->n_args[j] + 1) * sizeof(char*));

            // Tokenization of arguments

            char* saveptr3;
            char* token3 = strtok_r(token2, " ", &saveptr3);
            int k = 0;

            while(token3)
            {
                commands[i]->args[j][k] = strdup2(token3);
                k++;
                token3 = strtok_r(NULL, " ", &saveptr3);
            }
            commands[i]->args[j][commands[i]->n_args[j]] = NULL;

            j++;
            token2 = strtok_r(NULL, "|", &saveptr2);
        }

        i++;
        token1 = strtok_r(NULL, ";&", &saveptr1);
    }
    return commands;
}

void builtin_cd(char** args, int n_arg, const char* username)
{
    if(n_arg > 2)
    {
        printf("cd: too many arguments\n");
        return;
    }
    int res;
    if(n_arg == 1)
    {
        char home_path[strlen(username) + 7];
        sprintf(home_path, "/home/%s", username);
        res = chdir(home_path);
    }
    else if(n_arg == 2)
    {
        res = chdir(args[1]);
    }
    if(res == -1)
    {
        char error[strlen(args[1]) + 5];
        sprintf(error, "cd:%s ", args[1]);
        perror(error); // errno = chdir error
    }
}

void builtin_pwd(void)
{
    char pwd_buffer[PATH_MAX];
    if(getcwd(pwd_buffer, sizeof(pwd_buffer)) != NULL)
    {
        printf("%s\n", pwd_buffer);
    }
    else
    {
        perror("getcwd");
    }
}

int is_builtin_command(const char* arg0)
{
    return (!strcmp(arg0, "cd") ||
            !strcmp(arg0, "pwd") ||
            !strcmp(arg0, "exit") ||
            !strcmp(arg0, "jobs"));
}

int execute_builtin_command(char** args, int n_arg, Command* background_commands, int n_background_command, const char* username)
{
    if(!strcmp(args[0], "cd")) builtin_cd(args, n_arg, username);
    else if(!strcmp(args[0], "pwd")) builtin_pwd();
    else if(!strcmp(args[0], "exit")) return 0;
    else if(!strcmp(args[0], "jobs")) print_background_commands(background_commands, n_background_command);
    return 1;
}

void close_pipes(int (*pipes)[2], int n_pipe)
{
    for(int j = 0; j < n_pipe - 1; j++)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
}

int execute_command(Command* command, Command** p_background_commands, int* p_n_background_command, const char* username)
{
    // Initialization of pipes if the command contains any

    int (*pipes)[2];
    if(command->n_pipe > 1)
    {
        pipes = malloc((command->n_pipe - 1) * sizeof(int[2]));
        if(!pipes)
        {
            perror("Erreur malloc");
            return 1;
        }  
    }
    for(int i = 0; i < command->n_pipe - 1; i++)
    {
        if(pipe(pipes[i]) == -1)
        {
            perror("Erreur pipe");
            free(pipes);
            return 1;
        }
    }

    pid_t pids[command->n_pipe];

    for(int i = 0; i < command->n_pipe - command->is_file_output; i++)  // Execution of subcommands while properly configuring pipes
    {
        int is_builtin = is_builtin_command(command->args[i][0]);

        if(is_builtin && command->n_pipe == 1)  // Case where the command is builtin and does not contain a pipe
        {
            if(!execute_builtin_command(command->args[i], command->n_args[i], *p_background_commands, *p_n_background_command, username))
            {
                free_command(command);
                return 0;
            }
            free_command(command);
            return 1;
        }

        pid_t pid = fork(); // Fork to execute subcommands
        if(pid == -1)
        {
            perror("Erreur fork");
            if(command->n_pipe > 1) free(pipes);
            return 1;
        }
        if(pid == 0) // Child
        {
            if(command->is_file_output && (i == command->n_pipe - command->is_file_output - 1)) // If there is output redirection to a file
            {
                pipes[i][1] = open(command->args[i + 1][0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(pipes[i][1] < 0)
                {
                    perror("Erreur open");
                    close_pipes(pipes, command->n_pipe);   
                    exit(EXIT_FAILURE);
                }
            }

            // Pipe configurations

            int res1 = 0, res2 = 0;
            if(i != 0) res1 = dup2(pipes[i - 1][0], STDIN_FILENO);
            if(i != command->n_pipe - 1) res2 = dup2(pipes[i][1], STDOUT_FILENO);
            close_pipes(pipes, command->n_pipe);   
            if(res1 == -1 || res2 == -1)
            {
                perror("Erreur dup2");
                exit(EXIT_FAILURE);
            }

            // Execution of subcommands

            if(is_builtin)
            {
                execute_builtin_command(command->args[i], command->n_args[i], *p_background_commands, *p_n_background_command, username);
                exit(EXIT_SUCCESS);
            }
            else 
            {
                execvp(command->args[i][0], command->args[i]);
                perror(command->args[i][0]);
                if(errno == ENOENT) exit(127);
                else if(errno == EACCES) exit(126);
            }
        }
        pids[i] = pid;
    }
    
    if(command->n_pipe > 1) // Closing pipes and freeing memory
    {
        close_pipes(pipes, command->n_pipe);
        free(pipes);
    }

    if(command->is_background) // Registering the background command
    {
        command->pid = pids[command->n_pipe - command->is_file_output - 1];
        command->state = RUNNING;
        (*p_n_background_command)++;
        command->command_id = *p_n_background_command;
        add_background_command(p_background_commands, &command);
        printf("[%d] %d\n", command->command_id, command->pid);
    }
    else // If the command is not in the background, wait for all children
    {
        for(int i = 0; i < command->n_pipe - command->is_file_output; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
        free_command(command);
    }
    return 1;
}

void sigchild_handler(int sig) // sig is the received signal (only SIGCHLD for this function)
{
    (void)sig;
    int status;
    pid_t pid;

    // Wait for all finished children
    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        Command* command = g_background_commands;
        while(command)
        {
            if(command->pid == pid)
            {
                if(WEXITSTATUS(status) == 0) command->state = DONE;
                else if(WEXITSTATUS(status) == 126) command->state = EXIT_126;
                else command->state = EXIT_127;
                break;
            }
            command = command->next;
        }
    }
}