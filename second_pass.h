#ifndef SECOND_PASS_H
#define SECOND_PASS_H

/**
 * @file second_pass.h
 * @brief This header file declares functions and defines constants for the second pass of the assembler.
 *
 * The second pass is responsible for resolving symbol addresses and generating the final
 * machine code. The functions declared here handle the line-by-line processing of the
 * assembly file, updating the instruction memory with correct addresses for labels
 * and external references, and converting numeric values to the custom base-4 format.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "data.h"
#include "code.h"
#include "instruction.h"
#include "assembler.h"

/**
 * @def MEMORY_START
 * @brief The starting address for the program's memory space.
 *
 * All instruction and data addresses are calculated relative to this offset.
 */
#define MEMORY_START 100

/**
 * @brief Updates the final memory addresses for all labels.
 *
 * This function adjusts the memory index of each label in the symbol table based on the
 * final instruction and data counter values after the first pass. This ensures all
 * labels point to their correct, absolute memory locations.
 *
 * @param labeltable A pointer to the label memory table.
 * @param lac A pointer to the label counter, indicating the number of labels.
 * @param ic A pointer to the final instruction counter value.
 */
void index_update(struct labelMemory *labeltable, int *lac, int *ic);

/**
 * @brief Performs the second pass of the assembler line-by-line.
 *
 * It reads each line, identifies instructions and directives, and calls the appropriate
 * functions to resolve symbolic addresses (labels) and generate the complete machine code.
 * It also handles external and entry labels.
 *
 * @param str The line of assembly code to process.
 * @param instable A pointer to the instruction memory table.
 * @param labeltable A pointer to the label memory table.
 * @param errortable A pointer to the error table.
 * @param extable A pointer to the external labels table.
 * @param ec A pointer to the error counter.
 * @param lac A pointer to the label counter.
 * @param exsize A pointer to the size of the external table.
 * @param esize A pointer to the size of the error table.
 * @param exc A pointer to the external label counter.
 * @param cl_pass2 A pointer to the current line number for the second pass.
 * @param ic2 A pointer to the instruction counter for the second pass.
 * @return 1 on success, 0 on an error, or EXIT on a critical memory error.
 */
int second_pass(char str[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct error *errortable, struct external *extable, int *ec, int *lac, int *exsize, int *esize, int *exc, int *cl_pass2, int *ic2);

/**
 * @brief Updates instruction memory with resolved addresses for operands.
 *
 * This function parses the operands of an instruction. If an operand is a label
 * (direct or matrix), it calls `search_and_update` to find its final address
 * and write it into the instruction memory table.
 *
 * @param str The string containing the operands.
 * @param labeltable A pointer to the label memory table.
 * @param instable A pointer to the instruction memory table.
 * @param errortable A pointer to the error table.
 * @param extable A pointer to the external labels table.
 * @param ec A pointer to the error counter.
 * @param ic2 A pointer to the instruction counter for the second pass.
 * @param lac A pointer to the label counter.
 * @param exsize A pointer to the size of the external table.
 * @param esize A pointer to the size of the error table.
 * @param exc A pointer to the external label counter.
 * @param lc_pass2 A pointer to the current line number for the second pass.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int instruction_uptade_address(char str[], struct labelMemory *labeltable, struct instructionsMemory *instable, struct error *errortable, struct external *extable, int *ec, int *ic2, int *lac, int *exsize, int *esize, int *exc, int *lc_pass2);

/**
 * @brief Finds a label's address and updates the instruction memory.
 *
 * This function searches for a given label in the symbol table. If found, it populates
 * the instruction memory with the label's address and the appropriate relocation type
 * (Absolute, Relocatable, or External). If the label is not found, it logs an error.
 *
 * @param str The label name to search for.
 * @param instable A pointer to the instruction memory table.
 * @param labeltable A pointer to the label memory table.
 * @param extable A pointer to the external labels table.
 * @param errortable A pointer to the error table.
 * @param esize A pointer to the size of the error table.
 * @param exsize A pointer to the size of the external table.
 * @param exc A pointer to the external label counter.
 * @param lac A pointer to the label counter.
 * @param ic2 A pointer to the instruction counter for the second pass.
 * @param cl A pointer to the current line number.
 * @param ec A pointer to the error counter.
 * @return 1 on success, 0 if the label is not found, or EXIT on a critical memory error.
 */
int search_and_update(char str[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct external *extable, struct error *errortable, int *esize, int *exsize, int *exc, int *lac, int *ic2, int *cl, int *ec);

/**
 * @brief Converts a digit (0-3) to its special base-4 character representation ('a'-'d').
 *
 * This is a helper function for the `int_to_special_base4` conversion.
 *
 * @param digit The integer digit to convert (0-3).
 * @return The character representation of the digit.
 */
char digit_to_char_special_base4(int digit);

/**
 * @brief Adds one to a string representing a special base-4 number.
 *
 * This function is used to handle the two's complement-like logic for negative
 * numbers during base-4 conversion.
 *
 * @param base4_str The string representing the base-4 number.
 * @param len The length of the string.
 */
void add_one_to_special_base4(char* base4_str, int len);

/**
 * @brief Converts an integer to a string in a custom base-4 format.
 *
 * This function converts a signed integer to a string of 'a', 'b', 'c', 'd' characters.
 * It handles negative numbers by a method similar to two's complement and pads the
 * string to a specified number of digits.
 *
 * @param num The integer to convert.
 * @param num_digits The desired length of the output string.
 * @return A dynamically allocated string containing the base-4 representation, or NULL
 * if memory allocation fails.
 */
char* int_to_special_base4(int num, int num_digits);

#endif /* SECOND_PASS_H */
