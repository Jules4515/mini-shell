/**
 * @file main.c
 * @author Jules Guignard
 * @brief This file handles the main logic of the MiniShell
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "shell.h"

int main()
{
    signal(SIGCHLD, sigchild_handler); // Sets the signal handler for SIGCHLD

    char* username;
    char* username_hostname = get_username_hostname(&username); // Gets the username and hostname for display
    if(!username_hostname) return EXIT_FAILURE;
    
    // Infinite loop that asks the user for commands
    while(1)
    {
        if(!printf_username_wdpath(username_hostname, username))
        {
            free(username_hostname);
            return EXIT_FAILURE;
        }

        char* str_command = input_command();
        if(!str_command || !verify_format(str_command))
        {
            continue;
        }
        
        print_finished_commands(&g_background_commands, &g_n_background_command); // Display finished background commands
        
        int n_command;
        Command** commands = parser(str_command, &n_command);
        free(str_command);

        for(int i = 0; i < n_command; i++)
        {
            if(!execute_command(commands[i], &g_background_commands, &g_n_background_command, username))
            {
                free(username_hostname);
                free(commands);
                free_background_commands(&g_background_commands, &g_n_background_command);
                return EXIT_SUCCESS;
            }
        }
        free(commands);
    }
}