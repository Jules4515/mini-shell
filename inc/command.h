/**
 * @file command.h
 * @author Jules Guignard
 * @brief Header file, handles Commands
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/// @brief Enumeration defining the states of background Commands
typedef enum { RUNNING, DONE, EXIT_126, EXIT_127 } CommandState;

typedef struct Command
{
    char*** args;           /** Arguments */
    int n_pipe;             /** Number of pipes */
    int* n_args;            /** Number of arguments in each command separated by pipes */
    int is_file_output;     /** Variable indicating if the Command output is a file, 1 = yes */
    int is_background;      /** Variable indicating if the Command is in the background or not, 1 = background and 0 = foreground */
    pid_t pid;              /** PID of the process associated with the Command*/
    int command_id;         /** Command identifier (displayed in brackets)*/
    CommandState state;     /** Command state */
    struct Command *next;   /** Pointer to the next Command (linked list) */
} Command; /** Typedef of struct Command */

/// @brief Global variable representing the number of Commands running in the background
extern int g_n_background_command;
/// @brief Global variable representing the Commands running in the background, organized as a linked list
extern Command* g_background_commands;

/**
 * @brief Adds a Command to the head of the background Commands list
 * 
 * @param p_commands Pointer to the global variable g_background_commands (linked list of background Commands)
 * @param p_command Pointer to the Command to add
 */
void add_background_command(Command** p_commands, Command** p_command);

/**
 * @brief Displays the list of background Commands
 * 
 * @param background_commands Global variable g_background_commands 
 * @param n_background_command Global variable g_n_background_command
 */
void print_background_commands(Command* background_commands, const int n_background_command);

/**
 * @brief Displays finished background Commands and removes them from the global linked list g_background_commands
 * 
 * @param p_background_commands Pointer to the global variable g_background_commands
 * @param p_n_background_command Pointer to the global variable g_n_background_command
 */
void print_finished_commands(Command** p_background_commands, int* p_n_background_command);

/**
 * @brief Frees the memory of a Command object
 * 
 * @param command Command to free
 */
void free_command(Command* command);

/**
 * @brief Frees the memory of the background Commands list
 * 
 * @param p_background_commands Pointer to the global variable g_background_commands
 * @param p_n_background_command Pointer to the global variable g_n_background_command
 */
void free_background_commands(Command** p_background_commands, int* p_n_background_command);

#endif