/**
 * @file utils.c
 * @author Jules Guignard
 * @brief Source file, contains useful functions for copying strings
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "utils.h"

char* strdup2(const char* str1)
{
    if(!str1) return NULL;
    char* str2 = malloc(strlen(str1) + 1);
    if(!str2) return NULL;
    strncpy(str2, str1, strlen(str1) + 1);
    return str2;
}

char* strndup2(const char* str1, size_t n)
{
    if(!str1) return NULL;
    size_t len = strnlen(str1, n);
    char* str2 = malloc(len + 1);
    if(!str2) return NULL;
    strncpy(str2, str1, len);
    str2[len] = '\0';
    return str2;
}