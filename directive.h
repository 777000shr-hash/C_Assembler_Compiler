#ifndef DIRECTIVE_H
#define DIRECTIVE_H

/**
 * @file directive.h
 * @brief This header file declares functions and defines constants for processing directives.
 *
 * It provides the interface for handling assembly directives such as `.data`,
 * `.string`, and `.mat`. The functions declared here are responsible for parsing
 * arguments, validating data, and updating the program's data memory.
 */

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "code.h"
#include "assembler.h"

/**
 * @def DECIMAL
 * @brief The base for numeric conversions (decimal).
 */
#define DECIMAL 10

/**
 * @def MAX_VAL
 * @brief The maximum valid integer value for a data word.
 */
#define MAX_VAL 511

/**
 * @def MIN_VAL
 * @brief The minimum valid integer value for a data word.
 */
#define MIN_VAL -512

/**
 * @def NUTHING
 * @brief A placeholder constant, possibly for a default or unused value.
 */
#define NUTHING 1000

/**
 * @brief Processes a line containing a directive command.
 *
 * @param str The line of assembly code containing the directive.
 * @param datatable A pointer to the data memory table.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param dc A pointer to the data counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param dsize A pointer to the size of the data table.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int directive(char str[], struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize);

/**
 * @brief Handles the .data directive.
 *
 * @param word The string containing the numbers.
 * @param datatable A pointer to the data memory table.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param dc A pointer to the data counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param dsize A pointer to the size of the data table.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int data_update(char *word, struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize);

/**
 * @brief Handles the .mat directive.
 *
 * @param word The string containing the matrix definition.
 * @param datatable A pointer to the data memory table.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param dc A pointer to the data counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param dsize A pointer to the size of the data table.
 * @return 1 on success, 0 on an invalid matrix, or EXIT on a critical memory error.
 */
int mat_update(char word[], struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize);

/**
 * @brief Handles the .string directive.
 *
 * @param word The string literal to be processed.
 * @param datatable A pointer to the data memory table.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param dc A pointer to the data counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param dsize A pointer to the size of the data table.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int string_update(char *word, struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize);

/**
 * @brief Validates a string literal format.
 *
 * @param str The string to validate.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the string is valid, 0 if it's invalid, or EXIT on a memory error.
 */
int is_valid_string(char str[], struct error *errortable, int *ec, int *cl, int *esize);

/**
 * @brief Checks if a string-to-integer conversion was successful.
 *
 * @param str The original string that was converted.
 * @param endptr A pointer to the character that stopped the conversion.
 * @param val The integer value resulting from the conversion.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the conversion is valid, 0 if it failed or the value is out of range,
 * or EXIT on a memory error.
 */
int is_conversion_successful(char str[], char* endptr, int val, struct error *errortable, int *ec, int* cl, int *esize);

/**
 * @brief Checks for invalid number separators in a string.
 *
 * @param str The string to validate.
 * @return 1 if the numbers are validly separated, 0 otherwise.
 */
int is_valid_numbers(char str[]);

/**
 * @brief Checks if a string contains only spaces and tabs.
 *
 * @param str The string to check.
 * @return 1 if the string is empty or contains only spaces and tabs, 0 otherwise.
 */
int only_spaces_and_tabs(char str[]);

/**
 * @brief Finds the index of the first non-space character in a string.
 *
 * @param str The string to search.
 * @return The index of the first non-space character, or the string's length
 * if it contains only spaces.
 */
int begin_of_string(char str[]);

/**
 * @brief Finds the index of the character after the last non-space character.
 *
 * @param str The string to search.
 * @return The index after the last non-space character, or 0 if the string is
 * empty or contains only spaces.
 */
int end_of_string(char str[]);

/**
 * @brief Validates the format of a matrix definition string.
 *
 * @param str The string to validate.
 * @return 1 if the format is valid, 0 otherwise.
 */
int matrix_definition_valid(char str[]);

#endif /* DIRECTIVE_H */
