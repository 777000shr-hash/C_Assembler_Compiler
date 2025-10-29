#ifndef INSTRUCTION_H
#define INSTRUCTION_H

/**
 * @file instruction.h
 * @brief This header file defines constants, macros, and function prototypes
 * for handling assembly instructions during the first pass of the assembler.
 *
 * It includes declarations for functions that process instruction lines,
 * validate operands, and update the instruction memory table. It also defines
 * key constants for operand types, addressing modes, and opcodes, along with
 * macros to simplify the process of adding instruction words to memory.
 */

#include "data.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "directive.h"
#include "assembler.h"
#include "code.h"

/**
 * @def REGISTER
 * @brief A constant representing the register addressing mode.
 */
#define REGISTER 11

/**
 * @def IMMEDIATE
 * @brief A constant representing the immediate addressing mode.
 */
#define IMMEDIATE 00

/**
 * @def DIRECT
 * @brief A constant representing the direct addressing mode.
 */
#define DIRECT 01

/**
 * @def MATRIX
 * @brief A constant representing the matrix addressing mode.
 */
#define MATRIX 10

/**
 * @def ABSOLUTE
 * @brief A constant representing the absolute relocation type (A).
 */
#define ABSOLUTE 0

/**
 * @def EXTERNAL
 * @brief A constant representing the external relocation type (E).
 */
#define EXTERNAL 1

/**
 * @def RELOCATABLE
 * @brief A constant representing the relocatable relocation type (R).
 */
#define RELOCATABLE 2

/**
 * @def NO_OPERAND
 * @brief A constant to indicate the absence of an operand.
 */
#define NO_OPERAND -1

/**
 * @def NO_KNEWN
 * @brief A constant for an unknown or unassigned value.
 */
#define NO_KNEWN -4

/**
 * @def ERROR
 * @brief A constant for an error status.
 */
#define ERROR -2

/**
 * @def MAX_OPS
 * @brief The maximum number of operands an instruction can have.
 */
#define MAX_OPS 2

/**
 * @enum command
 * @brief An enumeration of all supported instruction opcodes.
 */
enum command{MOV=0,CMP,ADD,SUB,LEA,CLR,NOT,INC,DEC,JMP,BNE,JSR,RED,PRN,RTS, STOP};

/**
 * @brief A macro to add a 2-operand instruction to the instruction memory.
 *
 * This macro simplifies the process of adding the first word of a two-operand
 * instruction, which contains the opcode and operand types, to the instruction table.
 *
 * @param instable The instruction memory table.
 * @param idx A pointer to the instruction counter.
 * @param val The instruction's opcode.
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 * @param isize A pointer to the size of the instruction memory table.
 * @return 1 on success, or EXIT on a memory allocation failure.
 */
#define SET_OPCODE_2_AND_TYPE2(instable, idx, val, operand1, operand2, isize)\
do{\
	if(add_ins(&instable, idx, isize, RECORD_TYPE_COMMAND, val, operand1, operand2, 0, 0) == EXIT){return EXIT;}\
	return 1;\
}while(0);

/**
 * @brief A macro to add a 1-operand instruction to the instruction memory.
 *
 * This macro simplifies the process of adding the first word of a one-operand
 * instruction to the instruction table.
 *
 * @param instable The instruction memory table.
 * @param idx A pointer to the instruction counter.
 * @param val The instruction's opcode.
 * @param operand The type of the single operand.
 * @param isize A pointer to the size of the instruction memory table.
 * @return 1 on success, or EXIT on a memory allocation failure.
 */
#define SET_OPCODE_1_AND_TYPE1(instable, idx, val, operand, isize)\
do{\
	if(add_ins(&instable, idx, isize, RECORD_TYPE_COMMAND, val, 0, operand, 0, 0) == EXIT){return EXIT;}\
	return 1;\
}while(0);

/**
 * @brief A macro to add a 0-operand instruction to the instruction memory.
 *
 * This macro simplifies the process of adding the single word of a zero-operand
 * instruction to the instruction table.
 *
 * @param instable The instruction memory table.
 * @param idx A pointer to the instruction counter.
 * @param val The instruction's opcode.
 * @param isize A pointer to the size of the instruction memory table.
 * @return 1 on success, or EXIT on a memory allocation failure.
 */
#define SET_OPCODE_0_AND_TYPE0(instable, idx, val, isize)\
do{\
	if(add_ins(&instable, idx, isize, RECORD_TYPE_COMMAND, val, 0, 0, 0, 0) == EXIT){return EXIT;}\
	return 1;\
}while(0);

/**
 * @brief A macro to add a register operand word to the instruction memory.
 *
 * This macro is used for instructions with register operands. It creates a word
 * containing the source and destination register numbers and their relocation type.
 *
 * @param instable The instruction memory table.
 * @param idx A pointer to the instruction counter.
 * @param reg1 The first register number.
 * @param reg2 The second register number.
 * @param are The relocation type (Absolute, Relocatable, or External).
 * @param isize A pointer to the size of the instruction memory table.
 */
#define SET_REGISTE_AND_TYPE(instable, idx, reg1, reg2, are, isize)\
do{\
	if(add_ins(&instable, idx, isize, RECORD_TYPE_REGISTER, 0, reg1, reg2, are, 0) == EXIT){goto clean_up;}\
}while(0);

/**
 * @brief A macro to add an address or immediate value word to the instruction memory.
 *
 * This macro is used for instructions with direct or immediate operands. It
 * creates a word containing the operand's value and relocation type.
 *
 * @param instable The instruction memory table.
 * @param idx A pointer to the instruction counter.
 * @param add The value of the operand.
 * @param are The relocation type.
 * @param isize A pointer to the size of the instruction memory table.
 */
#define SET_ADDRESS_AND_TYPE(instable, idx, add, are, isize)\
do{\
	if(add_ins(&instable, idx, isize, RECORD_TYPE_ADDRESS, 0, 0, 0, are, add) == EXIT){goto clean_up;}\
}while(0);

/**
 * @brief A macro to check for a two-operand instruction.
 *
 * This macro verifies that an instruction expecting two operands has exactly
 * two operands. If not, it logs an error.
 *
 * @param errortable The error table.
 * @param ec_p A pointer to the error counter.
 * @param cl_p A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 */
#define OPERAND_2_CHECK(errortable, ec_p, cl_p, esize, operand1, operand2) \
do { \
	if (operand1 == NO_OPERAND || operand2 == NO_OPERAND) \
	{ \
		if (add_error(&errortable, ec_p, *cl_p, ": error! there must be 2 operands", esize) == EXIT) {return EXIT;}\
		return 0;\
	} \
} while(0);

/**
 * @brief A macro to check for an illegal immediate operand.
 *
 * This macro verifies that a specific operand is not of type IMMEDIATE, which
 * is a restriction for certain instructions. If it is, it logs an error.
 *
 * @param errortable The error table.
 * @param ec_p A pointer to the error counter.
 * @param cl_p A pointer to the current line number.
 * @param operand_p The operand type to check.
 * @param esize A pointer to the size of the error table.
 */
#define IMMEDIATE_IS_ERROR(errortable, ec_p, cl_p, operand_p, esize) \
do{ \
	if(operand_p == IMMEDIATE) \
	{ \
		if(add_error(&errortable, ec_p, *cl_p, ": error! illegal address in operand", esize) == EXIT){return EXIT;}\
		return 0;\
	}\
}while(0);

/**
 * @brief A macro to check for a one-operand instruction.
 *
 * This macro verifies that an instruction expecting one operand has exactly
 * one operand. If not, it logs an error.
 *
 * @param errortable The error table.
 * @param ec_p A pointer to the error counter.
 * @param cl_p A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 */
#define OPERAND_1_CHECK(errortable, ec_p, cl_p, esize, operand1, operand2) \
do{ \
	if(((operand1 == NO_OPERAND) && (operand2 == NO_OPERAND)) || ((operand1 != NO_OPERAND) && (operand2 != NO_OPERAND))) \
	{ \
		if(add_error(&errortable, ec_p, *cl_p, ": error! there must be 1 operand", esize) == EXIT){return EXIT;}\
		return 0;\
	}\
}while(0);

/**
 * @brief A macro to check for a zero-operand instruction.
 *
 * This macro verifies that an instruction expecting zero operands has none.
 * If any operands are found, it logs an error.
 *
 * @param errortable The error table.
 * @param ec_p A pointer to the error counter.
 * @param cl_p A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 */
#define OPERAND_0_CHECK(errortable, ec_p, cl_p, esize, operand1, operand2) \
do{ \
	if((operand1 != NO_OPERAND || operand2 != NO_OPERAND)) \
	{ \
		if(add_error(&errortable, ec_p, *cl_p, ": error! there must be 0 operands", esize) == EXIT){return EXIT;}\
		return 0;\
	}\
}while(0);


/**
 * @brief Dispatches an instruction based on its name and operand types.
 *
 * @param str The instruction name (e.g., "mov", "add").
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 * @param instable A pointer to the instruction memory table.
 * @param errortable A pointer to the error table.
 * @param ic A pointer to the instruction counter.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param isize A pointer to the size of the instruction memory table.
 * @param esize A pointer to the size of the error table.
 * @return 0 on success, or EXIT on a critical memory error.
 */
int word(char str[], int operand1, int operand2, struct instructionsMemory* instable, struct error* errortable, int* ic, int* ec, int* cl, int *isize, int *esize);

/**
 * @brief Determines the addressing type of an operand string.
 *
 * @param str The operand string.
 * @return An integer representing the operand type.
 */
int which_type_operand(char str[]);

/**
 * @brief Processes an instruction line during the first pass.
 *
 * @param str The instruction line.
 * @param instable A pointer to the instruction memory table.
 * @param errortable A pointer to the error table.
 * @param ic A pointer to the instruction counter.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param isize A pointer to the size of the instruction memory table.
 * @param esize A pointer to the size of the error table.
 * @return 1 on success, 0 on a syntax error, or EXIT on a critical memory error.
 */
int instruction(char str[], struct instructionsMemory *instable, struct error *errortable, int *ic, int *ec, int *lc, int *isize, int *esize);

/**
 * @brief Updates the instruction memory with operand information.
 *
 * @param word1 The first operand string.
 * @param word2 The second operand string (optional).
 * @param operand1 The type of the first operand.
 * @param operand2 The type of the second operand.
 * @param instable A pointer to the instruction memory table.
 * @param errortable A pointer to the error table.
 * @param ic A pointer to the instruction counter.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param isize A pointer to the size of the instruction memory table.
 * @param esize A pointer to the size of the error table.
 * @return 1 on success, 0 on an invalid operand, or EXIT on a critical memory error.
 */
int update(char word1[], char word2[], int operand1, int operand2, struct instructionsMemory* instable, struct error *errortable, int* ic, int* ec, int *lc, int *isize, int *esize);

/**
 * @brief Validates the format of a matrix operand.
 *
 * @param str The matrix operand string.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the matrix is valid, 0 if it's invalid, or EXIT on a critical memory error.
 */
int is_valid_matrix(char str[], struct error *errortable, int* ec, int *lc, int *esize);

/**
 * @brief Checks if a string contains matrix-style brackets.
 *
 * @param str The string to check.
 * @return 0 if brackets are found, 1 otherwise.
 */
int is_matrix(char str[]);

/**
 * @brief Parses and validates operand syntax, including commas and spaces.
 *
 * @param ops_str The string containing the operands.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the operand syntax is valid, 0 if it's invalid, or EXIT on a critical memory error.
 */
int parse_ops(char *ops_str, struct error *errortable, int *ec, int *lc, int *esize);

#endif /* INSTRUCTION_H */
