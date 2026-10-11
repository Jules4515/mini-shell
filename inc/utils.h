/**
 * @file utils.h
 * @author Jules Guignard
 * @brief Header file, contains useful functions for copying strings
 * @version 0.1
 * @date 2025-10-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Copies a string to another
 * 
 * @param str1 String to copy
 * @return Returns a new string equal to the first one
 */
char* strdup2(const char* str1);

/**
 * @brief Copies n characters from a string to another
 * 
 * @param str1 String to copy
 * @param n Number of characters to copy from the string
 * @return Returns a new string containing the first n characters of the first one
 */
char* strndup2(const char* str1, size_t n);

#endif