#include "second_pass.h"

/**
 * @brief Updates the index of all labels in the symbol table.
 *
 * This function is called after the first pass is complete. It iterates through the label
 * table and updates the final memory address for each label. Labels of type `DIRECTIVE`
 * have their index adjusted by the final instruction counter (IC) and the starting
 * memory address. Labels of type `INSTRUCTION` are adjusted only by the starting
 * memory address. This is crucial for correctly linking data segments to the
 * instruction segment.
 *
 * @param labeltable A pointer to the label memory table.
 * @param lac A pointer to the label counter.
 * @param ic A pointer to the final instruction counter.
 */
void index_update(struct labelMemory *labeltable, int *lac, int *ic)
{
	int i = 0;
	for(; i < *lac; i++)
	{
		if(labeltable[i].type == DIRECTIVE)
		{
			labeltable[i].index = labeltable[i].index + *ic + MEMORY_START;
		}
		else if(labeltable[i].type == INSTRUCTION)
		{
			labeltable[i].index = labeltable[i].index + MEMORY_START;
		}

	}
}

/**
 * @brief Performs the second pass of the assembler on a single line of code.
 *
 * This function's main purpose is to process instructions and update memory words
 * that contain addresses, such as labels. It identifies instructions and directives,
 * then calls helper functions to find and replace placeholder addresses with
 * their final, resolved memory locations. It also handles external and entry labels.
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
int second_pass(char str[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct error *errortable, struct external *extable, int *ec, int *lac, int *exsize, int *esize, int *exc, int *cl_pass2, int *ic2)
{
	char *word1 = NULL;
	char *word2 = NULL;
	int type, type2 = 0;
	const char *delimiters = " ,\t";
	char *str1 = NULL;
	char *str_copy = (char *)malloc(strlen(str) + 1);
	if(str_copy == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy \n");
		return EXIT;
	}
	strcpy(str_copy, str);
	word1 = strtok(str_copy, delimiters);
	word2 = strtok(NULL, delimiters);
	type = which_type(word1, errortable, ec, cl_pass2, NOT_UPDATE, esize);
	if(word2 != NULL)
	{
		type2 = which_type(word2, errortable, ec, cl_pass2, NOT_UPDATE, esize);
	}
	if(type == EXIT || type2 == EXIT){goto clean_second;}
	if(type == -1)
	{
		type = type2;
	}
	if(type == INSTRUCTION)
	{
		if(is_label(word1))
		{
			word1 = string_without_first_word(str, delimiters);
			if(str1 != NULL && strcmp(word2, EXIT_C) == 0){goto clean_second;}
			str1 = string_without_first_word(word1, delimiters);
			if(str1 != NULL && strcmp(str1, EXIT_C) == 0){goto clean_second;}
		}
		else
		{
			str1 = string_without_first_word(str, " \t");
			if(str1 != NULL && strcmp(str1, EXIT_C) == 0){goto clean_second;}
		}
		if(str1 != NULL)
		{
			if(instruction_uptade_address(str1, labeltable, instable, errortable, extable, ec, ic2, lac, exsize, esize, exc, cl_pass2) == EXIT){goto clean_second;}
		}
		free(str1);
		if(is_label(word1))
		{
			free(word1);
		}
	}
	else if(type == DIRECTIVE && is_label(word1) == 0)
	{
		word1[strlen(word1) - 1] = '\0';
		if(search_and_update(word1, instable, labeltable,extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_second;}
	}
	else if(type == EXTERN || type == ENTRY)
	{
		free(str_copy);
		return 1;
	}
	else
	{
		free(str_copy);
		return 0;
	}
	free(str_copy);
	return 1;

	clean_second:
		free(str_copy);
		return EXIT;
}

/**
 * @brief Updates the addresses for instruction operands.
 *
 * This function processes the operand section of an instruction line. It
 * identifies operands that are labels or matrices and calls `search_and_update`
 * to find their final memory address in the label table and update the
 * corresponding instruction memory words. It also correctly increments the
 * instruction counter (`ic2`) based on the number of words generated for
 * each operand.
 *
 * @param str The operand string.
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
 * @param cl_pass2 A pointer to the current line number for the second pass.
 * @return 1 on success, or EXIT on a critical memory error.
 */
int instruction_uptade_address(char str[], struct labelMemory *labeltable, struct instructionsMemory *instable, struct error *errortable, struct external *extable, int *ec, int *ic2, int *lac, int *exsize, int *esize, int *exc, int *cl_pass2)
{
	char *word1 = NULL;
	char *word2 = NULL;
	char *word3 = NULL;
	char *word4 = NULL;
	const char *delimiters = ",";
	const char *delimiters_mat = " []\t";
	char *str_copy = (char *)malloc(strlen(str) + 1);
	int end1, end2;
	if(str_copy == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy \n");
		return EXIT;
	}
	strcpy(str_copy, str);
	word1 = strtok(str_copy, delimiters);
	word2 = strtok(NULL, delimiters);
	if(word1 != NULL)
	{
		end1 = end_of_string(word1);
		if(strlen(word1) != end1)
		{
			word1[end1] = '\0';
		}
	}
	if(word2 != NULL)
	{
		end2 = end_of_string(word2);
		if(strlen(word2) != end2)
		{
			word2[end2] = '\0';
		}
	}
	(*ic2)++;
	if(word1 != NULL && (which_type_operand(word1) == MATRIX || which_type_operand(word1) == DIRECT))
	{
		if(which_type_operand(word1) == MATRIX)
		{
			word3 = strtok(word1, delimiters_mat);
			if(search_and_update(word3, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2) = (*ic2) + 2;
		}
		else
		{
			word3 = word1;
			if(search_and_update(word3, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2)++;
		}
		if(word2 != NULL && which_type_operand(word2) == MATRIX)
		{
			word4 = strtok(word2, delimiters_mat);
			if(search_and_update(word4, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2) = (*ic2) + 2;
		}
		else if(word2 != NULL && which_type_operand(word2) == DIRECT)
		{
			if(search_and_update(word2, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2)++;
		}
		else if(word2 != NULL)
		{
			(*ic2)++;
		}
	}
	else if(word1 != NULL)
	{
		(*ic2)++;
		if(word2 != NULL && which_type_operand(word2) == MATRIX)
		{
			word4 = strtok(word2, delimiters_mat);
			if(search_and_update(word4, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2) = (*ic2) + 2;
		}
		else if(word2 != NULL && which_type_operand(word2) == DIRECT)
		{
			if(search_and_update(word2, instable, labeltable, extable, errortable, esize, exsize, exc, lac, ic2, cl_pass2, ec) == EXIT){goto clean_ins_up;}
			(*ic2)++;
		}
		else if(word2 != NULL)
		{
			(*ic2)++;
		}
	}
	if(which_type_operand(word1) == REGISTER && which_type_operand(word2) == REGISTER)
	{
		(*ic2)--;
	}
	free(str_copy);
	return 1;

	clean_ins_up:
		free(str_copy);
		return EXIT;
}

/**
 * @brief Searches for a label and updates the instruction memory with its address.
 *
 * This function looks up a label name in the label table. If found, it updates the
 * instruction memory word at the current instruction counter (`ic2`) with the label's
 * final address and its relocation type (relocatable, external, or absolute). If the label
 * is external, it also adds it to the external table. If the label is not found,
 * it reports an error.
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
 * @param cl_pass2 A pointer to the current line number for the second pass.
 * @param ec A pointer to the error counter.
 * @return 1 on success, 0 if the label is not found, or EXIT on a critical memory error.
 */
int search_and_update(char str[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct external *extable, struct error *errortable, int *esize, int *exsize, int *exc, int *lac, int *ic2, int *cl_pass2, int *ec)
{
	int i = 0;
	for(; i < *lac; i++)
	{
		if(strcmp(labeltable[i].name, str) == 0)
		{
			instable[*ic2].type = RECORD_TYPE_ADDRESS;
			if(labeltable[i].en == EXTERN)
			{
				add_extern(str, &extable, exsize, exc, *ic2 + MEMORY_START);
				instable[*ic2].data.addr.address = 0;
				instable[*ic2].data.addr.ARE = EXTERNAL;
				return 1;
			}
			else if(labeltable[i].en == ENTRY)
			{
				instable[*ic2].data.addr.address = labeltable[i].index;
				instable[*ic2].data.addr.ARE = RELOCATABLE;
				return 1;
			}
			else
			{
				instable[*ic2].data.addr.address = labeltable[i].index;
				instable[*ic2].data.addr.ARE = RELOCATABLE;
				return 1;
			}
		}
	}
	if(add_error(&errortable, ec, *cl_pass2, ": error! Label name is not defined", esize) == EXIT){return EXIT;}
	return 0;
}

/**
 * @brief Converts a digit (0-3) to its special base-4 character representation ('a'-'d').
 *
 * This is a helper function for the base-4 conversion.
 *
 * @param digit The integer digit to convert (0-3).
 * @return The character representation of the digit.
 */
char digit_to_char_special_base4(int digit)
{
	switch (digit)
	{
		case 0: return 'a';
		case 1: return 'b';
		case 2: return 'c';
		case 3: return 'd';
		default: return '\0';
	}
}

/**
 * @brief Adds one to a special base-4 number represented as a string.
 *
 * This function is used to handle the two's complement representation for negative numbers
 * in the base-4 conversion. It performs a carry-add operation on the string.
 *
 * @param base4_str The string representing the base-4 number.
 * @param len The length of the string.
 */
void add_one_to_special_base4(char* base4_str, int len)
{
	int carry = 1;
	int i;
	int digit;
	for (i = len - 1; i >= 0 && carry > 0; i--)
	{
		digit = 0;
		switch (base4_str[i])
		{
			case 'a': digit = 0; break;
			case 'b': digit = 1; break;
			case 'c': digit = 2; break;
			case 'd': digit = 3; break;
		}
		digit += carry;
		carry = digit / 4;
		base4_str[i] = digit_to_char_special_base4(digit % 4);
	}
}

/**
 * @brief Converts an integer to a special base-4 string representation.
 *
 * This function converts a signed integer to a string of 'a', 'b', 'c', 'd' characters,
 * representing a base-4 number. It handles both positive and negative numbers by using a
 * two's complement-like logic for negative values. The output string is padded with 'a's
 * to a specified length.
 *
 * @param num The integer to convert.
 * @param num_digits The desired length of the output string.
 * @return A dynamically allocated string containing the base-4 representation,
 * or NULL if memory allocation fails.
 */
char* int_to_special_base4(int num, int num_digits)
{
	char* base4_str;
	int is_negative = (num < 0);
	int current_num;
	int i;
	int k;
	int digit;
	base4_str = (char*)malloc(num_digits + 1);
	if (base4_str == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed for base4_str.\n");
		return NULL;
	}
	memset(base4_str, 'a', num_digits);
	base4_str[num_digits] = '\0';

	if (is_negative)
	{
		current_num = abs(num);
		i = num_digits - 1;
		while (current_num > 0 && i >= 0)
		{
			base4_str[i--] = digit_to_char_special_base4(current_num % 4);
			current_num /= 4;
		}
		for (k = 0; k < num_digits; k++)
		{
			digit = 0;
			switch (base4_str[k])
			{
				case 'a': digit = 0; break;
				case 'b': digit = 1; break;
				case 'c': digit = 2; break;
				case 'd': digit = 3; break;
			}
			base4_str[k] = digit_to_char_special_base4(3 - digit);
		}
		add_one_to_special_base4(base4_str, num_digits);
	}
	else
	{
		current_num = num;
		i = num_digits - 1;
		if (current_num == 0)
		{
		}
		else
		{
			while (current_num > 0 && i >= 0)
			{
				base4_str[i--] = digit_to_char_special_base4(current_num % 4);
				current_num /= 4;
			}
		}
	}
	return base4_str;
}
