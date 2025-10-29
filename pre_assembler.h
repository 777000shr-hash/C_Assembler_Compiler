#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

/* Standard headers for C programming */
#include <stdio.h>      /* For standard input/output functions like fopen, fclose, fprintf */
#include <stdlib.h>     /* For memory management functions like malloc, free, and exit */
#include <string.h>     /* For string manipulation functions like strcmp, strlen, strdup */
#include <ctype.h>      /* For character handling functions like isspace, isalpha */

/* Definition of Constants */
#define MAX_LINE_LENGTH 80              /* The maximum allowed length for a line of code */
#define MAX_LABEL_LENGTH 30             /* The maximum allowed length for a label */
#define MACRO_START_KEYWORD "mcro"      /* The keyword that marks the beginning of a macro definition */
#define MACRO_END_KEYWORD "mcroend"     /* The keyword that marks the end of a macro definition */
#define COMMENT_START_CHAR ';'          /* The character used to indicate a comment line */
#define CODE_COLUMN 12                  /* A specific column for code formatting */
#define MAX_LINE_FOR_COLUMN_CHECK 1000  /* The maximum number of lines to check for column formatting */

/*
 * Structure to represent a single line within a macro.
 * This structure contains the line's content and a pointer to the next line, forming a linked list.
 */
typedef struct MacroLine
{
	char *content;
	struct MacroLine *next;
} MacroLine;

/*
 * Structure to manage a macro definition.
 * This structure holds the macro's name and a linked list of its lines.
 */
typedef struct MacroDefinition
{
	char name[MAX_LABEL_LENGTH + 1];
	MacroLine *head;        /* Pointer to the first line of the macro */
	MacroLine *tail;        /* Pointer to the last line of the macro for efficient appending */
	struct MacroDefinition *next;   /* Pointer to the next macro in the linked list */
} MacroDefinition;

/*
 * Additional includes for data structures and assembler functions.
 */
#include "data.h"
#include "assembler.h"

/*
 * Function Prototypes for the Pre-Assembler
 */

/**
 * @brief Duplicates a string.
 *
 * @param src The source string to duplicate.
 * @return A pointer to the newly duplicated string.
 */
char *my_strdup(const char *src);

/**
 * @brief Allocates memory for the macro table.
 *
 * @return A pointer to the newly allocated macro table.
 */
MacroDefinition *allocated_macro_table();

/**
 * @brief Checks if a given token is a reserved keyword in the assembly language.
 *
 * @param token The string to check.
 * @return 1 if the token is a reserved word, 0 otherwise.
 */
int is_reserved_word(const char *token);

/**
 * @brief Validates if a macro name is legal.
 *
 * @param name The macro name to validate.
 * @return 1 if the name is valid, 0 otherwise.
 */
int is_valid_macro_name(const char *name);

/**
 * @brief Adds a new macro definition to the linked list of macros.
 *
 * @param macros_list_head A pointer to the head of the macros linked list.
 * @param macro_def A pointer to the macro definition to be added.
 * @return 1 on success, or EXIT on a memory allocation failure.
 */
int add_macro_definition(MacroDefinition **macros_list_head, MacroDefinition *macro_def);

/**
 * @brief Finds a macro definition by its name.
 *
 * @param macros_list_head The head of the macros linked list.
 * @param name The name of the macro to find.
 * @return A pointer to the found macro, or NULL if not found.
 */
MacroDefinition *find_macro_definition(MacroDefinition *macros_list_head, const char *name);

/**
 * @brief Checks if a given word is already the name of an existing macro.
 *
 * @param macros_list_head The head of the macros linked list.
 * @param word The word to check.
 * @return 1 if the word is a macro name, 0 otherwise.
 */
int same_name_as_macro(MacroDefinition *macros_list_head, const char *word);

/**
 * @brief Frees all memory allocated for the macro definitions.
 *
 * @param macros_list_head A pointer to the head of the macros linked list.
 */
void free_macro_definitions(MacroDefinition **macros_list_head);

/**
 * @brief Trims leading/trailing whitespace and comments from a string.
 *
 * @param str The string to process.
 * @return A pointer to the processed string.
 */
char *trim_whitespace_and_comments(char *str);

/**
 * @brief Extracts the next token from a line of text.
 *
 * @param line_ptr A pointer to the current position in the line.
 * @return A pointer to the extracted token.
 */
char *get_next_token(char **line_ptr);

/**
 * @brief Adds a line of content to a specific macro definition.
 *
 * @param macro_def The macro to which the line will be added.
 * @param line_content The content of the line.
 * @return 1 on success, or EXIT on a memory allocation failure.
 */
int add_macro_line(MacroDefinition *macro_def, const char *line_content);

/**
 * @brief Prints a formatted line to a file.
 *
 * @param fp The file pointer.
 * @param label The label of the line (can be NULL).
 * @param text The text content of the line.
 */
void print_formatted_line(FILE *fp, const char *label, const char *text);

/**
 * @brief The main function of the pre-assembler.
 *
 * This function scans the input file, finds and stores macro definitions,
 * and expands them into an output file.
 *
 * @param input_filename The name of the input file.
 * @param errortable_ptr A pointer to the error table.
 * @param ec_ptr A pointer to the error counter.
 * @param esize_ptr A pointer to the size of the error table.
 * @param macros_list_head A pointer to the head of the macros linked list.
 * @return 1 on success, 0 if errors were found, or EXIT on a critical failure.
 */
int pre_assemble(const char *input_filename, struct error *errortable_ptr, int *ec_ptr, int *esize_ptr, MacroDefinition **macros_list_head);

#endif
