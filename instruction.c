#include "instruction.h"

/**
 * @brief Processes an assembly instruction and determines its opcode and operand types.
 *
 * This function acts as a dispatcher. It takes an instruction name and its
 * operand types, then calls the appropriate macros to set the opcode and
 * operand type information in the instruction memory table. It also
 * performs some basic operand validation based on the instruction's rules.
 *
 * @param str The instruction name (e.g., "mov", "add", "jmp").
 * @param operand1 The type of the first operand (IMMEDIATE, DIRECT, REGISTER, etc.).
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
int word(char str[], int operand1, int operand2, struct instructionsMemory* instable, struct error* errortable, int* ic, int* ec, int* cl, int *isize, int *esize)
{
	if(strcmp(str,"mov")==0)
	{
		OPERAND_2_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand2, esize);
		SET_OPCODE_2_AND_TYPE2(instable, ic, MOV, operand1, operand2, isize);
	}
	else if(strcmp(str,"cmp")==0)
	{
		OPERAND_2_CHECK(errortable, ec, cl, esize, operand1, operand2);
		SET_OPCODE_2_AND_TYPE2(instable, ic, CMP, operand1, operand2, isize);
	}
	else if(strcmp(str,"add")==0)
	{
		OPERAND_2_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand2, esize);
		SET_OPCODE_2_AND_TYPE2(instable, ic, ADD, operand1, operand2, isize);
	}
	else if(strcmp(str,"sub")==0)
	{
		OPERAND_2_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand2, esize);
		SET_OPCODE_2_AND_TYPE2(instable, ic, SUB, operand1, operand2, isize);
	}
	else if(strcmp(str,"lea")==0)
	{
		OPERAND_2_CHECK(errortable, ec, cl, esize, operand1, operand2);
		if(operand2 != IMMEDIATE && operand1 != IMMEDIATE && operand1 != REGISTER);
		{
			SET_OPCODE_2_AND_TYPE2(instable, ic, LEA, operand1, operand2, isize);
		}
		if(add_error(&errortable, ec, *cl, ": illegal address in operand", esize) == EXIT){return EXIT;}
		return 0;
	}
	else if(strcmp(str,"not")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, NOT, operand1, isize);
	}
	else if(strcmp(str,"clr")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, CLR, operand1, isize);
	}
	else if(strcmp(str,"inc")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, INC, operand1, isize);
	}
	else if(strcmp(str,"dec")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, DEC, operand1, isize);
	}
	else if(strcmp(str,"jmp")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, JMP, operand1, isize);
	}
	else if(strcmp(str,"bne")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, BNE, operand1, isize);
	}
	else if(strcmp(str,"red")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, RED, operand1, isize);
	}
	else if(strcmp(str,"prn")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		SET_OPCODE_1_AND_TYPE1(instable, ic, PRN, operand1, isize);
	}
	else if(strcmp(str,"jsr")==0)
	{
		OPERAND_1_CHECK(errortable, ec, cl, esize, operand1, operand2);
		IMMEDIATE_IS_ERROR(errortable, ec, cl, operand1, esize);
		SET_OPCODE_1_AND_TYPE1(instable, ic, JSR, operand1, isize);
	}
	else if(strcmp(str,"rts")==0)
	{
		OPERAND_0_CHECK(errortable, ec, cl, esize, operand1, operand2);
		SET_OPCODE_0_AND_TYPE0(instable, ic, RTS, isize);
	}
	else if(strcmp(str,"stop")==0)
	{
		OPERAND_0_CHECK(errortable, ec, cl, esize, operand1, operand2);
		SET_OPCODE_0_AND_TYPE0(instable, ic, STOP, isize);
	}
	return 0;
}

/**
 * @brief Determines the addressing type of an operand string.
 *
 * This function checks the format of an operand string to classify its addressing
 * type: immediate (#), register (r0-r7), matrix (label[rX][rY]), or direct label.
 *
 * @param str The operand string.
 * @return An integer representing the operand type (IMMEDIATE, REGISTER, etc.),
 * or NO_OPERAND if the string is null or empty.
 */
int which_type_operand(char str[])
{
	int i = 0;
	if(str == NULL || only_spaces_and_tabs(str))
	{
		return NO_OPERAND;
	}
	while (i < strlen(str) && isspace(str[i]))
	{
		i++;
	}
	if(str[i] == '#')
	{
		return IMMEDIATE;
	}
	else if(str[i] == 'r' && str[i + 1] >= '0' && str[i + 1] <= '7')
	{
		return REGISTER;
	}
	else if(is_matrix(str)==0)
	{
		return MATRIX;
	}
	return DIRECT;
}

/**
 * @brief Processes an instruction line during the first pass.
 *
 * This function parses an instruction line, identifies the instruction and its
 * operands, validates the operand syntax, and updates the instruction memory
 * table with the first word of the instruction and placeholders for operands.
 * It also handles memory allocation and error reporting.
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
int instruction(char str[], struct instructionsMemory *instable, struct error *errortable, int *ic, int *ec, int *lc, int *isize, int *esize)
{
	char *word1 = NULL;
	char *word2 = NULL;
	char *word3 = NULL;
	char *word4 = NULL;
	int operand1, operand2, temp;
	const char *delimiters = " ,\t";
	const char *delimiters2 = ",";
	char *str_copy = (char *)malloc(strlen(str) + 1);
	char *tmp = NULL;
	if (str_copy == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy \n");
		return EXIT;
	}
	strcpy(str_copy, str);
	word1 = strtok(str_copy, delimiters);
	word2 = strtok(NULL, delimiters2);
	word3 = strtok(NULL, delimiters2);
	word4 = strtok(NULL, delimiters);
	operand1 = which_type_operand(word2);
	operand2 = which_type_operand(word3);
	tmp = string_without_first_word(str, delimiters);
	if(tmp != NULL && strcmp(tmp, EXIT_C) == 0){goto clean_ins;}
	if(word2 == NULL)
	{
		operand1 = NO_OPERAND;
	}
	if(word3 == NULL)
	{
		operand2 = NO_OPERAND;
	}
	if(word4 != NULL)
	{
		if(add_error(&errortable, ec, *lc, ": error! More operands than allowed" , esize) == EXIT){goto clean_ins;}
		if(tmp)free(tmp);
		free(str_copy);
		return 0;
	}
	temp = parse_ops(tmp, errortable, ec, lc, esize);
	if(temp == EXIT){goto clean_ins;}
	else if(!temp)
	{
		if(tmp)free(tmp);
		free(str_copy);
		return 0;
	}
	temp = word(word1, operand1, operand2, instable, errortable, ic, ec, lc, isize, esize);
	if(temp)
	{
		if(word2 != NULL && operand1 != NO_OPERAND)
		{
			if(update(word2, word3, operand1, operand2, instable, errortable, ic, ec, lc, isize, esize) == EXIT){goto clean_ins;}
		}
		if(operand1 == REGISTER && operand2 == REGISTER){}
		else if(word3 != NULL && operand1 != NO_OPERAND)
		{
			if(update(word3, NULL , operand2, 0, instable, errortable, ic, ec, lc, isize, esize) == EXIT){goto clean_ins;}
		}
	}
	else if(temp == EXIT){goto clean_ins;}
	free(str_copy);
	if(tmp)free(tmp);
	return 1;

	clean_ins:
		free(str_copy);
		return EXIT;
		if(tmp)free(tmp);
}

/**
 * @brief Updates the instruction memory with operand information.
 *
 * This function is responsible for processing the operands and generating the
 * machine code words for them. It handles different addressing types (immediate,
 * direct, matrix, register) and populates the instruction memory table
 * accordingly. It reports errors for invalid operand formats.
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
int update(char word1[], char word2[], int operand1, int operand2, struct instructionsMemory* instable, struct error *errortable, int* ic, int* ec, int *lc, int *isize, int *esize)
{
	char *reg1mat = NULL;
	char *reg2mat = NULL;
	const char *delimitersMat = " []\t";
	const char *delim = " \tr#";
	char *str = (char *)malloc(strlen(word1) + 1);
	char *str1 = (char *)malloc(strlen(word1) + 1);
	char *str2 = NULL;
	char *str_p1 = NULL;
	char *str_p2 = NULL;
	char *ptr;
	int num;
	int valid;
	if (str1 == NULL || str == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy \n");
		goto clean_up;
	}
	strcpy(str, word1);
	strcpy(str1, word1);
	str_p1 = strtok(str1, delim);
	if(word2 != NULL)
	{
		str2 = (char *)malloc(strlen(word2) + 1);
		if(str2 == NULL)
		{
			fprintf(stdout,"Failed to allocate memory for string copy \n");
			goto clean_up;
		}
		strcpy(str2, word2);
		str_p2 = strtok(str2, delim);
	}
	switch(operand1)
	{
		case IMMEDIATE:
			num = strtol(str_p1, &ptr, DECIMAL);
			if(*ptr != '\0')
			{
				if(add_error(&errortable, ec, *lc, ": error! an immediate operand must contain a number." , esize) == EXIT){goto clean_up;}
				free(str);
				free(str1);
				if(str2)free(str2);
				return 0;
			}
			SET_ADDRESS_AND_TYPE(instable, ic, num, ABSOLUTE, isize)
			break;
		case DIRECT:
			SET_ADDRESS_AND_TYPE(instable, ic, NO_KNEWN, NO_KNEWN, isize)
			break;
		case MATRIX:
			valid = is_valid_matrix(str, errortable, ec, lc, esize);
			if(valid == EXIT){goto clean_up;}
			else if(!valid){goto clean_up_zero;}
			strtok(str, delimitersMat);
			reg1mat = strtok(NULL, delimitersMat);
			reg2mat = strtok(NULL, delimitersMat);
			memmove(reg1mat, reg1mat + 1, strlen(reg1mat));
			memmove(reg2mat, reg2mat + 1, strlen(reg2mat));
			SET_ADDRESS_AND_TYPE(instable, ic, NO_KNEWN, NO_KNEWN, isize)
			SET_REGISTE_AND_TYPE(instable, ic, atoi(reg1mat), atoi(reg2mat), ABSOLUTE, isize)
			break;
		case REGISTER:
			if(operand2 == REGISTER)
			{
				SET_REGISTE_AND_TYPE(instable, ic, atoi(str_p1), atoi(str_p2), ABSOLUTE, isize)
				break;
			}
			else if(word2 == NULL)
			{
				SET_REGISTE_AND_TYPE(instable, ic, 0, atoi(str_p1), ABSOLUTE, isize)
				break;
			}
			SET_REGISTE_AND_TYPE(instable, ic, atoi(str_p1), 0, ABSOLUTE, isize)
			break;
		default:
			if(add_error(&errortable, ec, *lc, ": error! unknown operand", esize) == EXIT){goto clean_up;}
			goto clean_up_zero;
			break;
	}
	free(str);
	free(str1);
	if(str2)free(str2);
	return 1;

	clean_up:
		if(str)free(str);
		if(str1)free(str1);
		if(str2)free(str2);
		return EXIT;
	clean_up_zero:
		if(str)free(str);
		if(str1)free(str1);
		if(str2)free(str2);
		return 0;
}

/**
 * @brief Validates the format of a matrix operand.
 *
 * This function checks if a string represents a valid matrix addressing mode.
 * The expected format is `label[rX][rY]`, where `rX` and `rY` are valid register names.
 * It reports errors for malformed matrix definitions.
 *
 * @param str The matrix operand string.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the matrix is valid, 0 if it's invalid, or EXIT on a critical memory error.
 */
int is_valid_matrix(char str[], struct error *errortable, int* ec, int *lc, int *esize)
{
	int i = 0;
	int len = 0;
	if (str == NULL)
	{
		return 0;
	}
	len = strlen(str);
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (!isalpha(str[i]))
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix. Matrix name must appear and begin with a letter" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i++;
	while (i < len && isalnum(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != '[')
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i++;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != 'r' || i + 1 >= len || !isdigit(str[i+1]) || (str[i+1] < '0' || str[i+1] > '7'))
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix a valid register must appear" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i += 2;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != ']')
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i++;
	if (i >= len || str[i] != '[')
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix" , esize) == EXIT){return EXIT;}
		return 1;
	}
	i++;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != 'r' || i + 1 >= len || !isdigit(str[i+1]) || (str[i+1] < '0' || str[i+1] > '7'))
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix a valid register must appear" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i += 2;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != ']')
	{
		if(add_error(&errortable, ec, *lc, ": error! Invalid matrix" , esize) == EXIT){return EXIT;}
		return 0;
	}
	i++;
	while (i < len)
	{
		if (!isspace(str[i]) && str[i] != ',')
		{
			if(add_error(&errortable, ec, *lc, ": error! Invalid matrix" , esize) == EXIT){return EXIT;}
			return 0;
		}
		i++;
	}

	return 1;
}

/**
 * @brief Checks if a string contains matrix-style brackets.
 *
 * This function scans a string to see if it contains square brackets, which
 * are characteristic of matrix addressing mode.
 *
 * @param str The string to check.
 * @return 0 if brackets are found (indicating a potential matrix), 1 otherwise.
 */
int is_matrix(char str[])
{
	int i = 0, begin = 0, end = 0;
	for(; i < strlen(str); i++)
	{
		if(str[i] == '[')
		{
			begin++;
		}
		else if(str[i] == ']')
		{
			end++;
		}
	}
	if(begin > 0 || end > 0)
	{
		return 0;
	}
	return 1;
}

/**
 * @brief Parses and validates operand syntax, including commas and spaces.
 *
 * This function checks for common syntax errors in the operand section of an
 * instruction line, such as incorrect placement of commas, extra spaces,
 * or multiple commas.
 *
 * @param ops_str The string containing the operands.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param lc A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the operand syntax is valid, 0 if it's invalid, or EXIT on a
 * critical memory error.
 */
int parse_ops(char *ops_str, struct error *errortable, int *ec, int *lc, int *esize)
{
	char *str = ops_str;
	char *comma_ptr = NULL;
	size_t len;
	char *space_ptr;
	char *last_bracket;
	char *op1_start;
	char *op1_end;
	char *op2_start;

	if(ops_str == NULL)
	{
		return 1;
	}
	while (isspace((unsigned char)*str))
	{
		str++;
	}
	len = strlen(str);
	while (len > 0 && isspace((unsigned char)str[len - 1]))
	{
		str[len - 1] = '\0';
		len--;
	}


	comma_ptr = strchr(str, ',');
	if (comma_ptr != NULL && strchr(comma_ptr + 1, ',') != NULL)
	{
		if(add_error(&errortable, ec, *lc, ": error! there must be only one comma between operands." , esize) == EXIT){return EXIT;}
		return 0;
	}


	if (len == 0)
	{
		return 1;
	}


	if (comma_ptr == NULL)
	{

		space_ptr = strchr(str, ' ');
		if (space_ptr != NULL)
		{
			last_bracket = strrchr(str, ']');
			if (last_bracket == NULL || space_ptr > last_bracket)
			{
				if(add_error(&errortable, ec, *lc, ": error! there must be a comma between operands.", esize) == EXIT){return EXIT;}
				return 0;
			}
		}
	}
	else
	{
		*comma_ptr = '\0';

		op1_start = str;
		while (isspace((unsigned char)*op1_start))
		{
			op1_start++;
		}
		op1_end = comma_ptr - 1;
		while (op1_end >= op1_start && isspace((unsigned char)*op1_end))
		{
			*op1_end = '\0';
			op1_end--;
		}
		if (strlen(op1_start) == 0)
		{
			if(add_error(&errortable, ec, *lc, ": error! a comma cannot be placed at the start or end of the line.", esize) == EXIT){return EXIT;}
			return 0;
		}


		op2_start = comma_ptr + 1;
		while (isspace((unsigned char)*op2_start))
		{
			op2_start++;
		}
		if (strlen(op2_start) == 0)
		{
			if(add_error(&errortable, ec, *lc, ": error! a comma cannot be placed at the start or end of the line.", esize) == EXIT){return EXIT;}
			return 0;
		}
	}

	return 1;
}
