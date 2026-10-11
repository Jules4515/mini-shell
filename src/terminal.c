/**
 * @file terminal.c
 * @author Jules Guignard
 * @brief Source file, handles input and display in the terminal
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "terminal.h"

char* input_command()
{
    char* command_buffer = malloc(COMMAND_SIZE_MAX);
    if(!command_buffer)
    {
        perror("Erreur malloc\n");
        return NULL;
    }

    if(fgets(command_buffer, COMMAND_SIZE_MAX, stdin) == NULL) // Command input by the user
    {
        if(feof(stdin))
        {
            fprintf(stderr, "Erreur EOF (fin de fichier atteinte)\n");
        }
        else if(ferror(stdin))
        {
            perror("Erreur de lecture avec fgets\n");
        }
        clearerr(stdin);
        free(command_buffer);
        return NULL;
    }

    if(strlen(command_buffer) == 1 && command_buffer[0] == '\n')
    {
        free(command_buffer);
        return NULL;
    }

    // Checks if the entered command exceeds the max buffer size
    // if so, empty the extra characters stored in the stdin stream and return an error
    if(strchr(command_buffer, '\n') == NULL)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        fprintf(stderr, "Erreur : ligne de commande trop longue\n");
        free(command_buffer);
        return NULL;
    }

    // Replaces the \n character (newline added in fgets when validating input) with \0 (null terminator)
    command_buffer[strcspn(command_buffer, "\n")] = '\0';

    return command_buffer;
}

int verify_format(char *str_command)
{
    if(!str_command) return 0;

    while(*str_command == ' ') str_command++;  // Remove leading spaces
    if(*str_command == '\0') return 0;

    int len = strlen(str_command);
    while(str_command[len - 1] == ' ') len--; // Verify that the last character (excluding space) is not a pipe
    if(str_command[len - 1] == '|') return 0;

    if(str_command[0] == ';' || str_command[0] == '&' || str_command[0] == '|') return 0; // Verify that the string does not start with |, ;, or &

    for(int i = 0; str_command[i + 1] != '\0'; i++) // Verify that there are no sequences of characters like |, ;, and &
    {
        if((str_command[i] == ';' || str_command[i] == '&' || str_command[i] == '|'))
        {
            while(str_command[i + 1] == ' ') i++;
            if(str_command[i + 1] == ';' || str_command[i + 1] == '&' || str_command[i + 1] == '|') return 0;
        }
    }
    return 1;
}

int printf_username_wdpath(char *username_hostname, char *username)
{
    printf("%s", username_hostname);
    char* work_dir_path = get_work_dir_path(username);
    if(!work_dir_path) return 0;
    printf("%s", work_dir_path);
    free(work_dir_path);
    return 1;
}

char* get_username_hostname(char** username)
{
    *username = getlogin();
    if(!(*username))
    {
        perror("Erreur getlogin\n");
        return NULL;
    }

    char hostname[80];
    if(gethostname(hostname, sizeof(hostname)))
    {
        perror("Erreur gethostname\n");
        return NULL;
    }

    // ANSI escape sequences add ~60 bytes of overhead
    size_t needed = strlen(*username) + strlen(hostname) + 80;
    char* username_hostname = malloc(needed);
    if(!username_hostname)
    {
        perror("malloc");
        return NULL;
    }
    snprintf(username_hostname, needed, "\033[1;38;2;0;150;50m(MiniShell)\033[0m \033[1;38;5;196m%s@%s\033[0m:", *username, hostname);
    return username_hostname;
}

char* get_work_dir_path(const char* username)
{
    char buffer[200];
    if(getcwd(buffer, sizeof(buffer)) == NULL)
    {
        perror("Erreur getcwd\n");
        return NULL;
    }

    char home_path[strlen(username) + 8];
    snprintf(home_path, sizeof(home_path), "/home/%s", username);

    char* display_path = NULL;

    // If the path starts with /home/<user>, replace it with ~
    if(strncmp(buffer, home_path, strlen(home_path)) == 0)
    {
        size_t len = strlen(buffer) - strlen(home_path) + 2;
        display_path = malloc(len);
        if(!display_path)
        {
            perror("Erreur malloc");
            return NULL;
        }
        snprintf(display_path, len, "~%s", buffer + strlen(home_path));
    }
    else
    {
        display_path = strdup2(buffer);
        if(!display_path)
        {
            printf("Erreur strdup2");
            return NULL;
        }
    }

    char* work_dir_path = malloc(strlen(display_path) + 40);
    if(!work_dir_path)
    {
        perror("Erreur malloc");
        free(display_path);
        return NULL;
    }
    sprintf(work_dir_path, "\033[1;38;2;255;0;255m%s\033[0m$ ", display_path);
    free(display_path);
    return work_dir_path;
}