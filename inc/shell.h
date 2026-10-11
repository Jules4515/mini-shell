/**
 * @file shell.h
 * @author Jules Guignard
 * @brief Header file, handles the execution of commands
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#ifndef SHELL_H
#define SHELL_H

#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "terminal.h"
#include "command.h"

/**
 * @brief Transforms the command entered by the user into a usable data structure
 * 
 * @param str_command Valid command entered by the user
 * @param n_command Pointer to the number of Commands in the return array
 * @return Returns an array of n_command Commands
 */
Command** parser(char* str_command, int* n_command);

/**
 * @brief Executes the builtin command cd, changes the working directory of the MiniShell
 * 
 * @param args Command arguments
 * @param n_arg Number of arguments
 * @param username Username
 */
void builtin_cd(char** args, int n_arg, const char* username);

/**
 * @brief Executes the builtin command pwd, displays the working directory
 * 
 */
void builtin_pwd(void);

/**
 * @brief Checks if a command is builtin (cd, pwd, exit or jobs)
 * 
 * @param arg0 Builtin command
 * @return Returns 1 if the command is builtin, 0 otherwise
 */
int is_builtin_command(const char* arg0);

/**
 * @brief Executes builtin commands
 * 
 * @param args Command arguments
 * @param n_arg Number of arguments
 * @param background_commands Global variable g_background_commands 
 * @param n_background_command Global variable g_n_background_command
 * @param username Username
 * @return Returns 1 if everything goes well and 0 if exit
 */
int execute_builtin_command(char** args, int n_arg, Command* background_commands, int n_background_command, const char* username);

/**
 * @brief Closes the pipes of a pipe array
 * 
 * @param pipes Pipe array
 * @param n_pipe Number of pipes
 */
void close_pipes(int (*pipes)[2], int n_pipe);

/**
 * @brief Executes commands
 * 
 * @param command Command to execute
 * @param p_background_commands Pointer to the global variable g_background_commands
 * @param p_n_background_command Pointer to the global variable g_n_background_command
 * @param username Username
 * @return Returns 1 if everything goes well and 0 if exit
 */
int execute_command(Command* command, Command** p_background_commands, int* p_n_background_command, const char* username);

/**
 * @brief Executes when the SIGCHLD signal is detected, waits for finished children and changes their state
 * 
 * @param sig Triggered signal, unused variable
 */
void sigchild_handler(int sig);

#endif