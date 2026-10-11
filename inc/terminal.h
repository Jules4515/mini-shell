/**
 * @file terminal.h
 * @author Jules Guignard
 * @brief Header file, handles input and display in the terminal
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/
#define _POSIX_C_SOURCE 200809L

#ifndef TERMINAL_H
#define TERMINAL_H

#include <unistd.h>

#include "utils.h"

/// @brief Macro defining the max size of a command entered by the user
#define COMMAND_SIZE_MAX 1000

/**
 * @brief Asks the user for a command input in the terminal
 * 
 * @return Returns the command as a character string 
 */
char* input_command();

/**
 * @brief Checks the format of the command entered by the user
 * 
 * @param str_command Command entered by the user
 * @return Returns 1 if the command is valid, 0 otherwise
 */
int verify_format(char* str_command);

/**
 * @brief Displays the username, hostname, and working directory
 * 
 * @param username_hostname Username concatenated with the hostname
 * @param username Username
 * @return Returns 1 if everything goes well, 0 otherwise
 */
int printf_username_wdpath(char* username_hostname, char* username);

/**
 * @brief Retrieves the username and hostname for display in the terminal
 * 
 * @param username Username
 * @return Returns a character string containing the username and hostname
 */
char* get_username_hostname(char** username);

/**
 * @brief Retrieves the current working directory
 * 
 * @param username Username
 * @return Returns a character string representing the current working directory
 */
char* get_work_dir_path(const char* username);

#endif