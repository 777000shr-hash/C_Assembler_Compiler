#ifndef FILE_H
#define FILE_H
/**
 * @file file.h
 * @brief This header file declares functions and defines constants for file I/O operations.
 *
 * It is primarily used for reading an entire text file into a dynamic array of strings,
 * handling potential errors such as file not found or line length issues.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data.h"

/**
 * @def MAX_SIZE_CHAR
 * @brief Defines the maximum size of a character buffer used to read a single line.
 *
 * The value 81 accounts for a maximum line length of 80 characters, plus a null terminator.
 */
#define MAX_SIZE_CHAR 81

/**
 * @brief Reads a file into a dynamically allocated array of strings.
 *
 * This function opens a specified file, reads its content line by line, and stores
 * each line in a dynamically allocated array of character pointers. It handles
 * dynamic resizing of the array and reports errors related to line length or
 * file I/O.
 *
 * @param name The name of the file to be read.
 * @param out_num_line A pointer to an integer that will store the total number of
 * lines successfully read.
 * @param errortable A pointer to the error table where any encountered errors will be logged.
 * @param ec A pointer to the error counter.
 * @param esize A pointer to the current allocated size of the error table.
 * @return A pointer to a dynamically allocated array of strings. Each string
 * represents a line from the file. Returns NULL if the file is empty, cannot be
 * opened, or if a memory allocation failure occurs.
 */
char ** read_file(const char *name, int *out_num_line, struct error *errortable, int *ec, int *esize);


#endif /* FILE_H */
