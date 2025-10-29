#ifndef CODE_H
#define CODE_H

/**
 * @file code.h
 * @brief This header file declares core functions and constants for the assembler's logic.
 *
 * It includes declarations for functions that handle label validation, command
 * type identification, and the main assembly passes. It also defines constants
 * that are used throughout the program to represent command types and statuses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "data.h"
#include "pre_assembler.h"
#include "instruction.h"
#include "directive.h"
#include "second_pass.h"
#include "file.h"
#include "assembler.h"

/**
 * @def INSTRUCTION
 * @brief Represents an instruction type command.
 */
#define INSTRUCTION 0

/**
 * @def DIRECTIVE
 * @brief Represents a directive type command.
 */
#define DIRECTIVE 1

/**
 * @def ENTRY
 * @brief Represents an entry directive.
 */
#define ENTRY 3

/**
 * @def EXTERN
 * @brief Represents an extern directive.
 */
#define EXTERN 4

/**
 * @def UPDATE
 * @brief A flag used to indicate that an error should be logged.
 */
#define UPDATE 100

/**
 * @def NOT_UPDATE
 * @brief A flag used to indicate that an error should not be logged.
 */
#define NOT_UPDATE 110

/**
 * @def ADDRESS_OF_EXTERN
 * @brief The address assigned to an external label.
 */
#define ADDRESS_OF_EXTERN 0

/**
 * @def EXIT_C
 * @brief A string constant used to indicate a critical exit status.
 */
#define EXIT_C "EXIT"

/**
 * @brief Validates a given label name.
 * @param str The string to be validated as a label.
 * @param word2 The second word on the line, typically the command.
 * @param errortable A pointer to the error table.
 * @param labeltable A pointer to the label memory table.
 * @param macrostable A pointer to the macro definitions table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param lac A pointer to the number of labels in the label table.
 * @return 1 if the label is valid, 0 if it's invalid, or EXIT on memory error.
 */
int valid_label(char str[], char word2[], struct error *errortable, struct labelMemory *labeltable, MacroDefinition *macrostable, int *ec, int *cl,  int *esize, int *lac);

/**
 * @brief Checks if a string is a label by looking for a colon at the end.
 * @param str The string to check.
 * @return 1 if the string is a label, 0 otherwise.
 */
int is_label(char str[]);

/**
 * @brief Determines the type of a command or directive string.
 * @param str The command string to check.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param check_for_valid A flag indicating whether to add an error if the type is not recognized.
 * @param esize A pointer to the size of the error table.
 * @return An integer representing the type (DIRECTIVE, INSTRUCTION, etc.), -1 if unrecognized, or EXIT on memory error.
 */
int which_type(char str[], struct error *errortable, int *ec, int *cl, int check_for_valid, int *esize);

/**
 * @brief Searches for and updates an entry label in the label table.
 * @param str The name of the entry label to search for.
 * @param labeltable A pointer to the label memory table.
 * @param errortable A pointer to the error table.
 * @param lac A pointer to the label counter.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param lasize A pointer to the size of the label table.
 * @param esize A pointer to the size of the error table.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int search_entery_and_update(char str[], struct labelMemory *labeltable, struct error *errortable, int *lac, int *ec, int *cl, int *lasize, int *esize);

/**
 * @brief Performs the first pass of the assembler.
 * @param str The line of assembly code to process.
 * @param instable A pointer to the instruction memory table.
 * @param datatable A pointer to the data memory table.
 * @param errortable A pointer to the error table.
 * @param labeltable A pointer to the label memory table.
 * @param extable A pointer to the external labels table.
 * @param macrostable A pointer to the macro definitions table.
 * @param lac A pointer to the label counter.
 * @param ic A pointer to the instruction counter.
 * @param dc A pointer to the data counter.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param exc A pointer to the external counter.
 * @param esize A pointer to the size of the error table.
 * @param lasize A pointer to the size of the label table.
 * @param exsize A pointer to the size of the external table.
 * @param dsize A pointer to the size of the data table.
 * @param isize A pointer to the size of the instruction table.
 * @return 1 on success, 0 on an invalid line (with error logged), or EXIT on a critical memory error.
 */
int first_pass(char str[], struct instructionsMemory *instable, struct dataMemory *datatable, struct error *errortable, struct labelMemory *labeltable, struct external *extable, MacroDefinition *macrostable, int *lac, int *ic, int *dc, int *ec, int *cl, int *exc, int *esize, int *lasize, int *exsize, int *dsize, int *isize);

/**
 * @brief Manages the two-pass assembly process.
 * @param namefile The name of the file to process (typically the .am file).
 * @param instable A pointer to the instruction memory table.
 * @param labeltable A pointer to the label memory table.
 * @param errortable A pointer to the error table.
 * @param extable A pointer to the external labels table.
 * @param datatable A pointer to the data memory table.
 * @param macrostable A pointer to the macro definitions table.
 * @param ic A pointer to the instruction counter.
 * @param dc A pointer to the data counter.
 * @param ec A pointer to the error counter.
 * @param lac A pointer to the label counter.
 * @param exc A pointer to the external counter.
 * @param isize A pointer to the size of the instruction table.
 * @param dsize A pointer to the size of the data table.
 * @param esize A pointer to the size of the error table.
 * @param lasize A pointer to the size of the label table.
 * @param exsize A pointer to the size of the external table.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int passes(char namefile[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct error *errortable, struct external *extable, struct dataMemory *datatable, MacroDefinition *macrostable, int *ic, int *dc, int *ec, int *lac, int *exc, int *isize, int *dsize, int *esize, int *lasize, int *exsize);

/**
 * @brief Extracts a string without its first word.
 * @param str The original string.
 * @param delims A string containing the delimiter characters.
 * @return A new dynamically allocated string with the first word removed, or NULL if
 * there is no text after the first word. Returns EXIT_C on memory allocation failure.
 */
char *string_without_first_word(char str[], const char *delims);

#endif /* CODE_H */
