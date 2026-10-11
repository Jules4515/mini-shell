/**
 * @file command.c
 * @author Jules Guignard
 * @brief Source file, handles Commands
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "command.h"

int g_n_background_command = 0;
Command* g_background_commands = NULL;

void add_background_command(Command** p_commands, Command** p_command)
{
    (*p_command)->next = *p_commands;
    *p_commands = *p_command;
}

void print_background_commands(Command* background_commands, const int n_background_command)
{
    printf("Nombre de commandes en arrière plan : %d\n", n_background_command);
    Command* command = background_commands;
    if(n_background_command == 0) return;
    while(command)
    {
        printf("[%d]   ", command->command_id);
        if(command->state == RUNNING) printf("RUNNING");
        else if(command->state == DONE) printf("DONE");
        else if(command->state == EXIT_126) printf("EXIT_126");
        else printf("EXIT_127");
        printf("\t");
        for(int i = 0; i < command->n_pipe; i++)
        {
            for(int j = 0; j < command->n_args[i] - 1; j++)
            {
                printf("%s ", command->args[i][j]);
            }
            if(i != command->n_pipe - 1) printf("| ");
        }
        printf("\t%d", command->pid);
        printf("\n");

        command = command->next;
    }
}

void print_finished_commands(Command** p_background_commands, int* p_n_background_command)
{
    Command* current_command = *p_background_commands;
    Command* previous_command = NULL;

    while(current_command)
    {
        Command* next_command = current_command->next;

        // If a command is marked as finished, display it and remove it from the list
        if(current_command->state != RUNNING)
        {
            if(previous_command) previous_command->next = current_command->next;
            else *p_background_commands = current_command->next;

            // Display the finished command

            printf("[%d]   ", current_command->command_id);
            if(current_command->state == DONE) printf("Done ");
            else if(current_command->state == EXIT_126) printf("Exit 126");
            else if(current_command->state == EXIT_127) printf("Exit 127");
            printf("\t\t\t\t");
            for(int i = 0; i < current_command->n_pipe; i++)
            {
                for(int j = 0; j < current_command->n_args[i]; j++)
                {
                    printf("%s ", current_command->args[i][j]);
                }
                if(i != current_command->n_pipe - 1) printf("| ");
            }
            printf("\n");
            
            (*p_n_background_command)--;
            free_command(current_command);
        }
        else previous_command = current_command;
        current_command = next_command;
    }
}

void free_command(Command* command)
{
    if(!command) return;
    if(command->args)
    {
        for(int i = 0; i < command->n_pipe; i++)
        {
            for(int j = 0; j < command->n_args[i]; j++)
            {
                free(command->args[i][j]);
                command->args[i][j] = NULL;
            }
            free(command->args[i]);
            command->args[i] = NULL;
        }
        free(command->n_args);
        free(command->args);
        command->args = NULL;
    }
    free(command);
    command = NULL;
}

void free_background_commands(Command** p_background_commands, int* p_n_background_command)
{
    Command* command = *p_background_commands;
    Command* next_command;
    while(command)
    {
        next_command = command->next;
        free_command(command);
        command = next_command;
    }
    *p_background_commands = NULL;
    *p_n_background_command = 0;
}