#include "code.h"

/**
 * @brief Validates a given label name.
 *
 * This function checks a string to ensure it is a valid label name according to
 * a set of rules. The rules include:
 * - Must not start with a digit or an underscore.
 * - Must not exceed a maximum length (MAX_SIZE_LABEL).
 * - Must contain only alphanumeric characters and underscores.
 * - Must not be a reserved word (instruction, directive, etc.).
 * - Must not be a macro name.
 * - Must not have been previously defined as an external or regular label.
 *
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
int valid_label(char str[], char word2[], struct error *errortable, struct labelMemory *labeltable, MacroDefinition *macrostable, int *ec, int *cl,  int *esize, int *lac)
{
	int i = 0, begin;
	begin = begin_of_string(str);
	if((str[begin] >= '0' && str[begin] <= '9') || str[begin] == '_')
	{
		if(add_error(&errortable, ec, *cl, ": error! Label starts with a digit or an underscore", esize) == EXIT){return EXIT;}
		return 0;
	}
	if(strlen(str) - begin > MAX_SIZE_LABEL)
	{
		if(add_error(&errortable, ec, *cl, ": error! Label too long (max 30 characters)", esize) == EXIT){return EXIT;}
		return 0;
	}
	for(; i < strlen(str); i++)
	{
		if(isalnum(str[i]) == 0 && str[i] != '_')
		{
			if(add_error(&errortable, ec, *cl, ": error! Label with non-alphanumeric characters", esize) == EXIT){return EXIT;}
			return 0;
		}
	}
	for(i = 0; i < *lac; i++)
	{
		if(strcmp(labeltable[i].name, str) == 0)
		{
			if(labeltable[i].en == ENTRY)
			{
				if(labeltable[i].type != 0 || labeltable[i].index != 0)
				{
					if(add_error(&errortable, ec, *cl, ": error! Label name already defined", esize) == EXIT){return EXIT;}
					return 0;
				}
			}
			else if(labeltable[i].en == EXTERN)
			{
				if(add_error(&errortable, ec, *cl, ": error! Label name already defined as external", esize) == EXIT){return EXIT;}
				return 0;
			}
			else
			{
				if(add_error(&errortable, ec, *cl, ": error! Label name already defined", esize) == EXIT){return EXIT;}
				return 0;
			}
		}
		
	}
	if(is_reserved_word(str) == 1)
	{
		if(add_error(&errortable, ec, *cl, ": error! The label name is a reserved word", esize) == EXIT){return EXIT;}
		return 0;
	}
	if(same_name_as_macro(macrostable, str) == 1)
	{
		if(add_error(&errortable, ec, *cl, ": error! The label name has already been defined as a macro", esize) == EXIT){return EXIT;}
		return 0;
	}
	return 1;
}

/**
 * @brief Checks if a string is a label by looking for a colon at the end.
 *
 * @param str The string to check.
 * @return 1 if the string is a label, 0 otherwise.
 */
int is_label(char str[])
{
	if(str == NULL)
	{
		return 0;
	}
	if(str[strlen(str)-1]==':')
	{
		return 1;
	}
	return 0;
}

/**
 * @brief Determines the type of a command or directive string.
 *
 * This function checks a given string against a list of known instructions,
 * directives, and entry/extern commands to classify its type.
 *
 * @param str The command string to check.
 * @param errortable A pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param cl A pointer to the current line number.
 * @param check_for_valid A flag indicating whether to add an error if the type is not recognized.
 * @param esize A pointer to the size of the error table.
 * @return An integer representing the type (DIRECTIVE, INSTRUCTION, ENTRY, EXTERN),
 * -1 if unrecognized, or EXIT on memory error.
 */
int which_type(char str[], struct error *errortable, int *ec, int *cl, int check_for_valid, int *esize)
{
	if (str == NULL)
	{
		if(add_error(&errortable, ec, *cl, ": error! Unrecognized command name", esize) == EXIT){return EXIT;}
		return -1;
	}
	if(strcmp(str,".data")==0||strcmp(str,".string")==0||strcmp(str,".mat")==0)
	{
		return DIRECTIVE;
	}
	else if(strcmp(str,"mov")==0||strcmp(str,"cmp")==0||strcmp(str,"add")==0||strcmp(str,"sub")==0||strcmp(str,"not")==0||strcmp(str,"clr")==0||strcmp(str,"lea")==0||strcmp(str,"inc")==0||strcmp(str,"dec")==0||strcmp(str,"jmp")==0||strcmp(str,"bne")==0||strcmp(str,"red")==0||strcmp(str,"prn")==0||strcmp(str,"jsr")==0||strcmp(str,"rts")==0||strcmp(str,"stop")==0)
	{
		return INSTRUCTION;
	}
	else if(strcmp(str,".entry")==0)
	{
		return ENTRY;
	}
	else if(strcmp(str,".extern")==0)
	{
		return EXTERN;
	}
	if(check_for_valid == UPDATE)
	{
		if(add_error(&errortable, ec, *cl, ": error! Unrecognized command name", esize) == EXIT){return EXIT;}
	}
	return -1;

}

/**
 * @brief Performs the first pass of the assembler.
 *
 * This function processes a single line of assembly code, identifying labels,
 * directives, and instructions. It populates the symbol table and calculates the
 * instruction and data counter (`ic` and `dc`) values.
 *
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
int first_pass(char str[], struct instructionsMemory *instable, struct dataMemory *datatable, struct error *errortable, struct labelMemory *labeltable, struct external *extable, MacroDefinition *macrostable, int *lac, int *ic, int *dc, int *ec, int *cl, int *exc, int *esize, int *lasize, int *exsize, int *dsize, int *isize)
{
	char *word = (char *)malloc(strlen(str) + 1);
	char *word1 = NULL;
	char *word2 = NULL;
	char *word3 = NULL;
	int type, i = 0;
	const char *delimiters = " ,\t", *delims = " \t";
    	char *str_copy = (char *)malloc(strlen(str) + 1);
	char *strptr = NULL;
	int valid;
	if(str_copy == NULL || word == NULL)
	{
		fprintf(stdout,"Failed to allocate memory for string copy \n");
		if (str_copy != NULL) free(str_copy);
		if (word != NULL) free(word);
		return EXIT;
	}
	strcpy(str_copy, str);
	strcpy(word, str);
	word1 = strtok(str_copy, delimiters);
	word2 = strtok(NULL, delimiters);
	while(word[i] != ':' && strlen(word) > i)
	{
		i++;
	}
	if(strlen(word) != i)
	{
		if(word[i] == ':' && word[i + 1] != ' ' && word[i + 1] != '\t')
		{
			if(add_error(&errortable, ec, *cl, ": error! there must be a space or tab after a label", esize) == EXIT){goto clean_first;}
			goto clean_and_return_zero;
		}
	}
	if(word1 != NULL && is_label(word1))
	{
		word1[strlen(word1)-1] = '\0';
		type = which_type(word2, errortable, ec, cl, UPDATE, esize);
		valid = valid_label(word1, word2, errortable, labeltable, macrostable, ec, cl, esize, lac);
		if(!valid|| type == -1){goto clean_and_return_zero;}
		else if(valid == EXIT || type == EXIT){goto clean_first;}
		if(type != EXTERN && type != ENTRY)
		{
			if(add_label(&labeltable, lac, word1, lasize, type, *ic, *dc) == EXIT){goto clean_first;}
		}
		strptr = string_without_first_word(str, delimiters);
		type = which_type(word2, errortable, ec, cl, UPDATE, esize);
		if(strcmp(strptr,EXIT_C) == 0 || type == EXIT){goto clean_first;}
	}
	else
	{
		type = which_type(word1, errortable, ec, cl, UPDATE, esize);
		if(type == EXIT)
		{
			goto clean_first;
		}
		strptr = (char *)malloc(strlen(str) + 1);
		if(strptr == NULL)
		{
			fprintf(stdout,"Failed to allocate memory\n");
			free(str_copy);
			free(word);
			return EXIT;
		}
		strcpy(strptr, str);
	}
	if(type == DIRECTIVE)
	{
		if(directive(strptr, datatable, errortable, ec, dc, cl, esize, dsize) == EXIT){goto clean_first;}
	}
	else if(type == INSTRUCTION)
	{
		if(instruction(strptr, instable, errortable, ic, ec, cl, isize, esize) == EXIT){goto clean_first;}
	}
	else if(type == EXTERN)
	{
		word1 = strtok(strptr, delims);
		word2 = strtok(NULL, delims);
		word3 = strtok(NULL, delims);
		if(word3 != NULL)
		{
			if(add_error(&errortable, ec, *cl, ": error! invalid external label", esize) == EXIT){goto clean_first;}
		}
		else
		{
			if(add_label(&labeltable, lac, word2, lasize, EXTERN, 0, 0) == EXIT){goto clean_first;}
		}
	}
	else if(type == ENTRY)
	{
		word1 = strtok(strptr, delims);
		word2 = strtok(NULL, delims);
		word3 = strtok(NULL, delims);
		if(word3 != NULL)
		{
			if(add_error(&errortable, ec, *cl, ": error! invalid enternal label", esize) == EXIT){goto clean_first;}
		}
		else
		{
			if(search_entery_and_update(word2, labeltable, errortable, lac, ec, cl, lasize, esize) == EXIT){goto clean_first;}
		}
	}
	free(word);
	free(strptr);
	free(str_copy);
	return 1;

	clean_first:
		if (word) free(word);
		if (strptr) free(strptr);
		if (str_copy) free(str_copy);
		return EXIT;
	clean_and_return_zero:
		if (word) free(word);
		if (strptr) free(strptr);
		if (str_copy) free(str_copy);
		return 0;

}

/**
 * @brief Manages the two-pass assembly process.
 *
 * This function orchestrates the assembly by first reading the entire file into memory.
 * It then calls `first_pass` for each line to build the symbol table and calculate
 * memory sizes. After updating symbol table indices, it calls `second_pass` to
 * encode the instructions and data.
 *
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
int passes(char namefile[], struct instructionsMemory *instable, struct labelMemory *labeltable, struct error *errortable, struct external *extable, struct dataMemory *datatable, MacroDefinition *macrostable, int *ic, int *dc, int *ec, int *lac, int *exc, int *isize, int *dsize, int *esize, int *lasize, int *exsize)
{
	int numline = 0, i = 0, ic2 = 0;
	char **command = read_file(namefile, &numline, errortable, ec, esize);
	if(command == NULL)
	{
		return EXIT;
	}
	for(; i < numline; i++)
	{
		if(command[i] != NULL)
		{
			if(!only_spaces_and_tabs(command[i]) && command[i][0] != ';')
			{
				if(first_pass(command[i], instable, datatable, errortable, labeltable, extable, macrostable, lac, ic, dc, ec, &i, exc, esize, lasize, exsize, dsize, isize) == EXIT){goto clean_command;}
			}
		}
	}
	index_update(labeltable, lac, ic);
	for(i = 0; i < numline; i++)
	{
		if(command[i] != NULL)
		{
			if(!only_spaces_and_tabs(command[i]) && command[i][0] != ';')
			{
				if(second_pass(command[i], instable, labeltable, errortable, extable, ec, lac, exsize, esize, exc, &i, &ic2) == EXIT){goto clean_command;}
			}
		}

	}
	for(i = 0; i < numline; i++)
	{
		free(command[i]);
	}
	free(command);
	return 1;

	clean_command:
			for(i = 0; i < numline; i++)
			{
				free(command[i]);
			}
			free(command);
			return EXIT;
}

/**
 * @brief Searches for and updates an entry label in the label table.
 *
 * If a label with the given name exists and is not already an ENTRY or EXTERN,
 * its type is updated to ENTRY. If the label does not exist, a new ENTRY label
 * is added to the table. An error is logged if the label is already defined
 * as an EXTERN or ENTRY.
 *
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
int search_entery_and_update(char str[], struct labelMemory *labeltable, struct error *errortable, int *lac, int *ec, int *cl, int *lasize, int *esize)
{
	int i = 0, flag = 0;
	for(; i < (*lac); i++)
	{
		if(strcmp(labeltable[i].name, str) == 0)
		{
			if(labeltable[i].en != ENTRY && labeltable[i].en != EXTERN)
			{
				labeltable[i].en = ENTRY;
				flag = 1;
			}
			else
			{
				if(add_error(&errortable, ec, *cl, ": error! invalid enternal label", esize) == EXIT)
				{
					return EXIT;
				}
			}
		}
	}
	if(flag == 0)
	{
		if(add_label(&labeltable, lac, str, lasize, ENTRY, 0, 0) == EXIT)
		{
			return EXIT;
		}
	}
	return 1;
}

/**
 * @brief Extracts a string without its first word.
 *
 * This function takes a string and a set of delimiters, finds the first word,
 * and returns a new string containing everything after that first word,
 * including leading whitespace.
 *
 * @param str The original string.
 * @param delims A string containing the delimiter characters.
 * @return A new dynamically allocated string with the first word removed, or NULL if
 * there is no text after the first word. Returns EXIT_C on memory allocation failure.
 */
char *string_without_first_word(char str[], const char *delims)
{
	const char *rest = NULL;
	char *new_str = NULL;
	size_t len;
	const char *p = str;
	while (*p != '\0' && strchr(delims, *p) != NULL)
	{
		p++;
	}
	len = strcspn(p, delims);
	if (len == strlen(p))
	{
		return NULL;
	}
	else
	{
		rest = p + len;
		while (*rest != '\0' && strchr(delims, *rest) != NULL)
		{
			rest++;
		}
		if (*rest != '\0')
		{
			new_str = (char *)malloc(strlen(rest) + 1);
			if(new_str == NULL)
			{
				fprintf(stdout,"Failed to allocate memory\n");
				return EXIT_C;
			}
			strcpy(new_str, rest);
			return new_str;
		}
	}
	return NULL;
}
