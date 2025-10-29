#ifndef DATA_H
#define DATA_H

/**
 * @file data.h
 * @brief This header file defines the core data structures and constants used by the assembler.
 *
 * It contains declarations for the tables that hold instructions, labels, errors, and data.
 * It also defines constants for memory sizes and field lengths, as well as function
 * prototypes for memory allocation, adding records to tables, and generating output files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * @def MAX_SIZE_MEMORY
 * @brief The initial size of memory tables for instructions, data, and errors.
 *
 * This value defines the starting capacity of dynamically allocated arrays.
 * The memory will be reallocated if it exceeds this size.
 */
#define MAX_SIZE_MEMORY 156

/**
 * @def MAX_SIZE_LABEL
 * @brief The maximum allowed length for a label name, including the null terminator.
 */
#define MAX_SIZE_LABEL 30

/**
 * @def MAX_SIZE_MASAGE
 * @brief The maximum allowed length for an error message string.
 */
#define MAX_SIZE_MASAGE 200

/**
 * @def DOUBLE
 * @brief A constant used for dynamically resizing memory tables.
 *
 * When a table reaches its capacity, it is reallocated to be twice its current size.
 */
#define DOUBLE 2

/**
 * @def NUM_5, NUM_4, NUM_2, NUM_1, NUM_8
 * @brief Constants representing the number of digits needed for custom base-4 conversions.
 *
 * These are used by the output functions to format the machine code correctly.
 */
#define NUM_5 5
#define NUM_4 4
#define NUM_2 2
#define NUM_1 1
#define NUM_8 8

/**
 * @enum RecordType
 * @brief An enumeration to differentiate between different types of machine code words.
 *
 * - `RECORD_TYPE_COMMAND`: The first word of an instruction, containing the opcode and operand types.
 * - `RECORD_TYPE_ADDRESS`: A word representing an address or an immediate value.
 * - `RECORD_TYPE_REGISTER`: A word representing a pair of register operands.
 */
enum RecordType
{
	RECORD_TYPE_COMMAND,
	RECORD_TYPE_ADDRESS,
	RECORD_TYPE_REGISTER
};

/**
 * @struct Command
 * @brief A structure to store the components of a command word.
 *
 * - `opcode`: The operation code of the instruction.
 * - `operand1`: The addressing type of the source operand.
 * - `operand2`: The addressing type of the destination operand.
 * - `ARE`: The relocation type (Always 0 for command words).
 */
struct Command
{
	unsigned int opcode;
	unsigned int operand1;
	unsigned int operand2;
	unsigned int ARE;
};

/**
 * @struct address
 * @brief A structure to store the components of an address word.
 *
 * - `address`: The memory address or immediate value.
 * - `ARE`: The relocation type (Absolute, Relocatable, or External).
 */
struct address
{
	unsigned int address;
	unsigned int ARE;
};

/**
 * @struct registers
 * @brief A structure to store the components of a register word.
 *
 * - `operand1`: The source register number.
 * - `operand2`: The destination register number.
 * - `ARE`: The relocation type (Always 0 for register words).
 */
struct registers
{
	unsigned int operand1;
	unsigned int operand2;
	unsigned int ARE;
};

/**
 * @struct instructionsMemory
 * @brief A structure to represent a single machine code word in the instruction table.
 *
 * - `type`: Specifies the type of record (`RecordType`).
 * - `data`: A union that holds one of the specific data structures (`Command`, `address`, or `registers`)
 * based on the `type` field.
 */
struct instructionsMemory
{
	enum RecordType type;
	union
	{
		struct Command command;
		struct address addr;
		struct registers reg;
	} data;
};

/**
 * @struct labelMemory
 * @brief A structure to store information about a label (symbol).
 *
 * - `name`: The name of the label.
 * - `type`: The type of the label (INSTRUCTION or DIRECTIVE).
 * - `index`: The memory index or address of the label.
 * - `en`: The entry/extern type of the label (NONE, ENTRY, or EXTERN).
 */
struct labelMemory
{
	char name[MAX_SIZE_LABEL + 1];
	int type;
	int index;
	int en;
};

/**
 * @struct error
 * @brief A structure to store information about a single error.
 *
 * - `error`: The error message.
 * - `line`: The line number where the error occurred.
 */
struct error
{
	char error[MAX_SIZE_MASAGE];
	int line;
};

/**
 * @struct external
 * @brief A structure to track external labels and their usage.
 *
 * - `name`: The name of the external label.
 * - `index`: The memory address where the external label is referenced.
 */
struct external
{
	char name[MAX_SIZE_LABEL + 1];
	int index;
};

/**
 * @struct dataMemory
 * @brief A structure to represent a single data word.
 *
 * - `address`: The integer value stored in the data segment.
 */
struct dataMemory
{
	unsigned int address;
};

#include "directive.h"
#include "second_pass.h"
#include "assembler.h"

/* --- Function Prototypes --- */

/**
 * @brief Allocates memory for the instruction memory table.
 * @return A pointer to the allocated table, or NULL if allocation fails.
 */
struct instructionsMemory* allocated_memory_table();

/**
 * @brief Allocates memory for the label table.
 * @return A pointer to the allocated table, or NULL if allocation fails.
 */
struct labelMemory* allocated_label_table();

/**
 * @brief Allocates memory for the error table.
 * @return A pointer to the allocated table, or NULL if allocation fails.
 */
struct error* allocated_error_table();

/**
 * @brief Allocates memory for the external labels table.
 * @return A pointer to the allocated table, or NULL if allocation fails.
 */
struct external* allocated_extern_table();

/**
 * @brief Allocates memory for the data memory table.
 * @return A pointer to the allocated table, or NULL if allocation fails.
 */
struct dataMemory* allocated_dataMemory_table();

/**
 * @brief Adds an error to the error table.
 * @return 1 on success, EXIT on reallocation failure.
 */
int add_error(struct error **errortable, int *ec, int line_number, const char *error_message, int *esize);

/**
 * @brief Adds a new label to the label table.
 * @return 1 on success, EXIT on reallocation failure.
 */
int add_label(struct labelMemory **labeltable, int *lac, char str[], int *lasize, int type, int ic, int dc);

/**
 * @brief Adds a data value to the data memory table.
 * @return 1 on success, EXIT on reallocation failure.
 */
int add_data(struct dataMemory **datatable, int val,int *dc,int *dsize);

/**
 * @brief Adds an instruction word to the instruction memory table.
 * @return 1 on success, EXIT on reallocation failure.
 */
int add_ins(struct instructionsMemory **instable, int *ic, int *isize, enum RecordType type, int val, int operand1, int operand2, int are, int address);

/**
 * @brief Adds an external label reference to the external table.
 * @return 1 on success, EXIT on reallocation failure.
 */
int add_extern(char str[], struct external **extable, int *exsize, int *exc, int index);

/**
 * @brief Prints all collected errors to standard output.
 * @param errortable A pointer to the error table.
 * @param ec The number of errors.
 */
void print_error(struct error *errortable, int ec);

/**
 * @brief Generates and prints the `.ent` (entry) output file.
 * @return 1 on success, EXIT on file or memory error.
 */
int print_entry(char name[], struct labelMemory *labeltable, int *lac);

/**
 * @brief Generates and prints the `.ext` (external) output file.
 * @return 1 on success, EXIT on file or memory error.
 */
int print_extern(char name[], struct external *extable, int exc);

/**
 * @brief Generates and prints the `.ob` (object) output file.
 * @return 1 on success, EXIT on file or memory error.
 */
int print_object(char name[], struct instructionsMemory *instable, struct dataMemory *datatable, int ic, int dc);

/**
 * @brief Checks if any entry labels exist in the label table.
 * @return 1 if an entry label exists, 0 otherwise.
 */
int have_entry(struct labelMemory *labeltable, int *lac);

/**
 * @brief Searches for a label and handles special types (ENTRY/EXTERN).
 * @return 1 if the label is found and updated, 0 otherwise.
 */
int search_label(struct labelMemory *labeltable, int *lac, char *str, int *lasize, int type, int ic, int dc);

#endif
