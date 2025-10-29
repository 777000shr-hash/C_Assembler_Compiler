#include "directive.h"

/**
 * @brief Processes a line containing a directive command.
 *
 * This function parses a line to identify the specific directive type (`.data`,
 * `.string`, or `.mat`) and then calls the appropriate helper function to handle
 * the directive's arguments. It also handles memory allocation and error checking.
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
int directive(char str[], struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize)
{
	char *word1, *word2;
	const char *delimiters = " \t";
	char *str_cpy = (char *)malloc(strlen(str) + 1);
	if (str_cpy == NULL)
	{
		fprintf(stdout,"Failed to allocate memory\n");
		return EXIT;
	}
	strcpy(str_cpy, str);
	word1 = strtok(str_cpy, delimiters);
	word2 = string_without_first_word(str, delimiters);
	if(strcmp(word2, EXIT_C) == 0){goto clean_directive;}
	if(strcmp(word1, ".data") == 0)
	{
		if(data_update(word2, datatable, errortable, ec, dc, cl, esize, dsize) == EXIT){goto clean_directive;}
	}
	else if(strcmp(word1, ".string") == 0)
	{
		if(string_update(word2, datatable, errortable, ec, dc, cl, esize, dsize) == EXIT){goto clean_directive;}
	}
	else if(strcmp(word1, ".mat") == 0)
	{
		if(mat_update(word2, datatable, errortable, ec, dc, cl, esize, dsize) == EXIT){goto clean_directive;}
	}
	else
	{
		if(add_error(&errortable, ec, *cl, ": error! unknown directive command name", esize) == EXIT){goto clean_directive;}
	}
	if(word2)free(word2);
	if(str_cpy)free(str_cpy);
	return 1;

	clean_directive:
			if(word2)free(word2);
			if(str_cpy)free(str_cpy);
			return EXIT;
}

/**
 * @brief Handles the .data directive.
 *
 * This function parses a comma-separated list of numbers, validates them,
 * and adds each number to the data memory table. It reports errors for invalid
 * number formats or out-of-range values.
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
int data_update(char *word, struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize)
{
	char *str = NULL;
	char *delimiters = "	 ,", *endptr = NULL;
	int val, tmp, end;
	char *str_cpy = NULL;
	if(word != NULL && !only_spaces_and_tabs(word))
	{
		str_cpy = (char *)malloc(strlen(word) + 1);
		if (str_cpy == NULL)
		{
			fprintf(stdout,"Failed to allocate memory\n");
			return EXIT;
		}
		strcpy(str_cpy, word);
		end = end_of_string(str_cpy);
		str_cpy[end] = '\0';
		if(!(is_valid_numbers(str_cpy)))
		{
			if(add_error(&errortable, ec, *cl, ": error! invalid data string", esize) == EXIT){goto clean_data;}
		}
		else
		{
			str = strtok(str_cpy, delimiters);
			while(str != NULL)
			{

				val = strtol(str, &endptr, DECIMAL);
				tmp =  is_conversion_successful(str, endptr, val, errortable, ec, cl, esize);
				if(tmp == 0)
				{
					break;
				}
				if(tmp == EXIT){goto clean_data;}
				if(add_data(&datatable, val, dc, dsize) == EXIT){goto clean_data;}
				str = strtok(NULL, delimiters);
			}
		}
	}
	else
	{
		if(add_error(&errortable, ec, *cl, ": error! invalid data string, data string should have values", esize) == EXIT){goto clean_data;}
	}
	free(str_cpy);
	return 1;

	clean_data:
		free(str_cpy);
		return EXIT;
}

/**
 * @brief Handles the .mat directive.
 *
 * This function parses a matrix definition, validates its dimensions and optional
 * initial values, and adds the corresponding data to the data memory table.
 * It reports errors for an ill-defined matrix, invalid dimensions, or too many
 * initial values.
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
int mat_update(char word[], struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize)
{
	char *str1 = NULL;
	char *str2 = NULL;
	char *str3 = NULL, *str4 = NULL;
	char *delimiters = "	 ,[]", *endptr1, *endptr2, *endptr;
	int val1, val2, num = 0, val, tmp, end;
	char *str_cpy =  (char *)malloc(strlen(word) + 1);
	int succ1, succ2;
	if (str_cpy == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy");
		return EXIT;
	}
	strcpy(str_cpy, word);
	str3 = string_without_first_word(word, delimiters);
	str4 = string_without_first_word(str3, delimiters);
	str1 = strtok(str_cpy, delimiters);
	if(strcmp(str3,EXIT_C) == 0 || strcmp(str4,EXIT_C) == 0){goto clean_mat;}
	if(str1 == NULL)
	{
		if(add_error(&errortable, ec, *cl, ": error! invalid data matrix", esize) == EXIT){goto clean_mat;}
		goto clean;
	}
	str2 = strtok(NULL, delimiters);
	if(matrix_definition_valid(word))
	{
		val1 = strtol(str1, &endptr1, DECIMAL);
		val2 = strtol(str2, &endptr2, DECIMAL);
		succ1 = is_conversion_successful(str1, endptr1, val1, errortable, ec, cl, esize);
		succ2 = is_conversion_successful(str2, endptr2, val2, errortable, ec, cl, esize);
		if(succ1 == 1 && succ2 == 1)
		{
			if(str4 == NULL)
			{
				while(num != (val1 * val2))
				{
					if(add_data(&datatable, 0, dc, dsize) == EXIT){goto clean_mat;}
					num++;
				}
			}
			else
			{
				end = end_of_string(str4);
				str4[end] = '\0';
				if(!is_valid_numbers(str4))
				{
					if(add_error(&errortable, ec, *cl, ": error! invalid numbers string", esize) == EXIT){goto clean_mat;}
					goto clean;
				}
				str1 = strtok(str4, delimiters);
				while(str1 != NULL && num < (val1 * val2))
				{
					val = strtol(str1, &endptr, DECIMAL);
					tmp =  is_conversion_successful(str1, endptr, val, errortable, ec, cl, esize);
					if(!tmp){goto clean;}
					else if(tmp == EXIT){goto clean_mat;}
					if(add_data(&datatable, val, dc, dsize) == EXIT){goto clean_mat;}
					num++;
					str1 = strtok(NULL, delimiters);
				}
				if(str1 != NULL && num > (val1 * val2))
				{
					if(add_error(&errortable, ec, *cl, ": error! more values than specified", esize) == EXIT){goto clean_mat;}
					goto clean;
				}
				if(num < (val1 * val2))
				{
					while(num <= (val1 * val2))
					{
						if(add_data(&datatable, 0, dc, dsize) == EXIT){goto clean_mat;}
						num++;
					}
				}
				free(str3);
				free(str4);
				free(str_cpy);
				return 1;
			}
		}
		if(succ1 == EXIT || succ2 == EXIT){goto clean_mat;}
		goto clean;
	}
	else
	{
		if(add_error(&errortable, ec, *cl, ": error! an ill-defined matrix", esize) == EXIT){goto clean_mat;}
		goto clean;
	}
	clean:
		free(str_cpy);
		free(str3);
		free(str4);
		return 0;
	clean_mat:
		free(str_cpy);
		free(str3);
		free(str4);
		return EXIT;
}

/**
 * @brief Handles the .string directive.
 *
 * This function validates a string literal, extracts its characters, and adds
 * each character's ASCII value to the data memory table. It also adds a null
 * terminator at the end. It reports errors for invalid string formats.
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
int string_update(char *word, struct dataMemory *datatable, struct error *errortable, int *ec, int *dc, int* cl, int *esize, int *dsize)
{
	int i = 1, end, valid;
	end = end_of_string(word);
	valid = is_valid_string(word, errortable, ec, cl, esize);
	if(valid == 1)
	{
		while(i < (end -1))
		{
			if(add_data(&datatable, (int)word[i], dc, dsize) == EXIT){return EXIT;}
			i++;
		}
		if(add_data(&datatable, '\0', dc, dsize) == EXIT){return EXIT;}

	}
	if(valid == EXIT){return EXIT;}
	return 1;
}

/**
 * @brief Validates a string literal format.
 *
 * Checks if a string literal starts and ends with double quotes (`"`) and
 * contains only printable ASCII characters.
 *
 * @param str The string to validate.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param esize A pointer to the size of the error table.
 * @return 1 if the string is valid, 0 if it's invalid, or EXIT on a memory error.
 */
int is_valid_string(char str[], struct error *errortable, int *ec, int *cl, int *esize)
{
	int i;
	int end = end_of_string(str);
	int begin = begin_of_string(str);
	if(str[begin] != '"' || str[end-1] != '"')
	{
		if(add_error(&errortable, ec, *cl, ": error! String must start and end with quotes", esize) == EXIT){return EXIT;}
		return 0;
	}
	for(i = begin + 1; i < end; i++)
	{
		if(str[i] > '~' || str[i] < ' ')
		{
			if(add_error(&errortable, ec, *cl, ": error! illegal characters in a string", esize) == EXIT){return EXIT;}
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Checks if a string-to-integer conversion was successful.
 *
 * Validates the result of `strtol`. It checks if the entire string was consumed
 * by the conversion, and if the resulting value is within a defined range
 * (`MIN_VAL` to `MAX_VAL`).
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
int is_conversion_successful(char str[], char* endptr, int val, struct error *errortable, int *ec, int* cl, int *esize)
{
	if (endptr == str || *endptr != '\0')
	{
		if(add_error(&errortable, ec, *cl, ": error! invalid characters", esize) == EXIT){return EXIT;}
		return 0;
	}
	if (val > MAX_VAL || val < MIN_VAL)
	{
		if(add_error(&errortable, ec, *cl, ": error! the value is too large or too small", esize) == EXIT){return EXIT;}
		return 0;
	}
	return 1;
}

/**
 * @brief Checks for invalid number separators in a string.
 *
 * This function validates that a string of numbers separated by commas does not
 * have consecutive commas or a comma at the beginning or end.
 *
 * @param str The string to validate.
 * @return 1 if the numbers are validly separated, 0 otherwise.
 */
int is_valid_numbers(char str[])
{
	int i = 0, len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if(str[i] == ',' && str[i+1] == ',')
		{
			return 0;
		}
	}
	if(str[0] == ',' || str[len -1] == ',')
	{
		return 0;
	}
	return 1;
}

/**
 * @brief Checks if a string contains only spaces and tabs.
 *
 * @param str The string to check.
 * @return 1 if the string is empty or contains only spaces and tabs, 0 otherwise.
 */
int only_spaces_and_tabs(char str[])
{
	int i = 0;
	for(; i < strlen(str); i++)
	{
		if(!isspace(str[i]))
		{
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Finds the index of the first non-space character in a string.
 *
 * @param str The string to search.
 * @return The index of the first non-space character, or the string's length
 * if it contains only spaces.
 */
int begin_of_string(char str[])
{
	int i = 0;
	if(only_spaces_and_tabs(str))
	{
		return 0;
	}
	for(; i < strlen(str); i++)
	{
		if(str[i] != ' ' && str[i] != '\t')
		{
			return i;
		}
	}
	return i;
}

/**
 * @brief Finds the index of the character after the last non-space character.
 *
 * @param str The string to search.
 * @return The index after the last non-space character, or 0 if the string is
 * empty or contains only spaces.
 */
int end_of_string(char str[])
{
	int i = strlen(str) - 1;
	if(only_spaces_and_tabs(str))
	{
		return 0;
	}
	for(; i >= 0 ; i--)
	{
		if(str[i] != ' ' && str[i] != '\t')
		{
			return i + 1;
		}
	}
	return i;
}

/**
 * @brief Validates the format of a matrix definition string.
 *
 * This function checks if a string matches the expected format for a matrix
 * definition: `[<number>][<number>]`.
 *
 * @param str The string to validate.
 * @return 1 if the format is valid, 0 otherwise.
 */
int matrix_definition_valid(char str[])
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
	if (i >= len || str[i] != '[')
	{
		return 0;
	}
	i++;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || !isdigit(str[i]))
	{
		return 0;
	}
	while (i < len && isdigit(str[i]))
	{
		i++;
	}
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != ']')
	{
		return 0;
	}
	i++;
	if (i >= len || str[i] != '[')
	{
		return 0;
	}
	i++;
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || !isdigit(str[i]))
	{
		return 0;
	}
	while (i < len && isdigit(str[i]))
	{
		i++;
	}
	while (i < len && isspace(str[i]))
	{
		i++;
	}
	if (i >= len || str[i] != ']')
	{
		return 0;
	}
	i++;
	if (!isspace(str[i]))
	{
		return 0;
	}
	i++;
	return 1;
}
