#include "pre_assembler.h"

/**
 * @var RESERVED_WORDS
 * @brief An array of strings containing all reserved keywords in the assembly language.
 *
 * This includes all instruction names, directives (e.g., .data, .string), and register names.
 * This list is used to ensure that user-defined macros and labels do not conflict with
 * language keywords.
 */
const char *RESERVED_WORDS[] =
{
	"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp",
	"bne", "red", "prn", "jsr", "rts", "stop", "data", "string", "mat", "entry", "extern",
	".data", ".string", ".mat", ".entry", ".extern", "mcro", "mcroend",
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", NULL
};

/**
 * @brief Duplicates a string on the heap.
 *
 * This function is a custom implementation of `strdup`. It allocates memory for
 * a new string and copies the content of the source string into it. It's used
 * to manage dynamic memory for strings.
 *
 * @param src The source string to duplicate.
 * @return A pointer to the newly allocated and duplicated string, or NULL if
 * memory allocation fails.
 */
char *my_strdup(const char *src)
{
	char *dup = malloc(strlen(src) + 1);
	if (dup) strcpy(dup, src);
	return dup;
}

/**
 * @brief Allocates and initializes the macro table.
 *
 * This function is currently a placeholder and returns `NULL` directly, as the macro
 * table is managed as a linked list and the head pointer is initialized to `NULL`
 * in the calling function.
 *
 * @return Always returns NULL.
 */
MacroDefinition *allocated_macro_table()
{
	return NULL;
}

/**
 * @brief Checks if a token is a reserved word.
 *
 * This function iterates through the `RESERVED_WORDS` array and compares the
 * input token with each reserved keyword.
 *
 * @param token The string to check.
 * @return 1 if the token is a reserved word, 0 otherwise.
 */
int is_reserved_word(const char *token)
{
	int i;
	for (i = 0; RESERVED_WORDS[i] != NULL; i++)
	{
		if (strcmp(token, RESERVED_WORDS[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Validates a macro name.
 *
 * A valid macro name must start with a letter and can only contain
 * alphanumeric characters and underscores.
 *
 * @param name The macro name to validate.
 * @return 1 if the name is valid, 0 otherwise.
 */
int is_valid_macro_name(const char *name)
{
	int i;
	if (!isalpha((unsigned char)name[0]))
	{
		return 0;
	}
	for (i = 1; name[i] != '\0'; i++)
	{
		if (!isalnum((unsigned char)name[i]) && name[i] != '_')
		{
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Adds a new macro definition to the macro list.
 *
 * This function prepends a new `MacroDefinition` node to the head of the
 * linked list of macros.
 *
 * @param macros_list_head A pointer to the head of the macro list.
 * @param macro_def A pointer to the macro definition to add.
 * @return Always returns 0.
 */
int add_macro_definition(MacroDefinition **macros_list_head, MacroDefinition *macro_def)
{
	macro_def->next = *macros_list_head;
	*macros_list_head = macro_def;
	return 0;
}

/**
 * @brief Finds a macro definition by its name.
 *
 * This function searches the linked list of macros for a macro with a matching name.
 *
 * @param macros_list_head The head of the macro list.
 * @param name The name of the macro to find.
 * @return A pointer to the found `MacroDefinition`, or NULL if not found.
 */
MacroDefinition *find_macro_definition(MacroDefinition *macros_list_head, const char *name)
{
	MacroDefinition *current = macros_list_head;
	while (current != NULL)
	{
		if (strcmp(current->name, name) == 0)
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}

/**
 * @brief Checks if a word is the name of a defined macro.
 *
 * This is a wrapper function that uses `find_macro_definition`.
 *
 * @param macros_list_head The head of the macro list.
 * @param word The word to check.
 * @return 1 if the word is a macro name, 0 otherwise.
 */
int same_name_as_macro(MacroDefinition *macros_list_head, const char *word)
{
	if (find_macro_definition(macros_list_head, word) != NULL)
	{
		return 1;
	}
	return 0;
}

/**
 * @brief Frees all memory allocated for the macro definitions.
 *
 * This function iterates through the macro list and frees all macro
 * definitions and their associated lines, preventing memory leaks.
 *
 * @param macros_list_head A pointer to the head of the macro list.
 */
void free_macro_definitions(MacroDefinition **macros_list_head)
{
	MacroDefinition *current_macro = *macros_list_head;
	MacroLine *current_line;
	MacroLine *temp_line;
	MacroDefinition *temp_macro;
	while (current_macro != NULL)
	{
		current_line = current_macro->head;
		while (current_line != NULL)
		{
			temp_line = current_line;
			current_line = current_line->next;
			free(temp_line->content);
			free(temp_line);
		}
		temp_macro = current_macro;
		current_macro = current_macro->next;
		free(temp_macro);
	}
	*macros_list_head = NULL;
}

/**
 * @brief Trims leading/trailing whitespace and removes comments from a string.
 *
 * It first null-terminates the string at the beginning of any comment character
 * (`;`). Then it removes any leading and trailing whitespace from the remaining string.
 *
 * @param str The string to process.
 * @return A pointer to the trimmed string, or NULL if the string becomes empty.
 */
char *trim_whitespace_and_comments(char *str)
{
	char *end;

	/* Find and terminate at the comment character */
	char *comment_start = strchr(str, COMMENT_START_CHAR);
	if(comment_start != NULL)
	{
		*comment_start = '\0';
	}

	/* Trim leading space */
	while (isspace((unsigned char)*str)) str++;

	if(*str == '\0')
	{
		return NULL;
	}

	/* Trim trailing space */
	end = str + strlen(str) - 1;
	while (end >= str && isspace((unsigned char)*end))
	{
		end--;
	}
	*(end + 1) = '\0';

	return str;
}


/**
 * @brief Extracts the next token from a line of code.
 *
 * This function tokenizes a line of code based on whitespace and a colon.
 * It modifies the input string by null-terminating the token and updating the
 * line pointer to the start of the next token. This is a destructive operation.
 *
 * @param line_ptr A pointer to a pointer to the current position in the line.
 * @return A pointer to the extracted token, or NULL if no more tokens are found.
 */
char *get_next_token(char **line_ptr)
{
	char *start = *line_ptr;
	char *end;
	char *token_end_temp;

	/* Skip leading whitespace */
	while (*start != '\0' && isspace((unsigned char)*start))
	{
		start++;
	}
	if (*start == '\0')
	{
		*line_ptr = start;
		return NULL;
	}

	end = start;

	/* Find the end of the token, stopping at whitespace or a colon */
	while (*end != '\0' && !isspace((unsigned char)*end) && *end != ':')
	{
		end++;
	}

	/* If a colon is found, the token is a label */
	if (*end == ':')
	{
		*end = '\0';
		*line_ptr = end + 1;
		return start;
	}
	else
	{
		token_end_temp = end;
		*line_ptr = end;

		while (*token_end_temp != '\0' && isspace((unsigned char)*token_end_temp))
		{
			token_end_temp++;
		}
		*line_ptr = token_end_temp;

		*end = '\0';
		return start;
	}
}


/**
 * @brief Adds a line of content to a macro definition.
 *
 * This function allocates a new `MacroLine` node, duplicates the content,
 * and appends it to the end of the macro's linked list of lines.
 *
 * @param macro_def A pointer to the macro definition.
 * @param line_content The content of the line to add.
 * @return 1 on success, or EXIT on memory allocation failure.
 */
int add_macro_line(MacroDefinition *macro_def, const char *line_content)
{
	MacroLine *new_line = (MacroLine *)malloc(sizeof(MacroLine));
	if (!new_line)
	{
		fprintf(stdout,"Memory allocation failed");
		return EXIT;
	}

	new_line->content = my_strdup(line_content);
	if (!new_line->content)
	{
		fprintf(stdout,"Memory allocation failed");
		free(new_line);
		return EXIT;
	}
	new_line->next = NULL;

	if (macro_def->head == NULL)
	{
		macro_def->head = new_line;
		macro_def->tail = new_line;
	}
	else
	{
		macro_def->tail->next = new_line;
		macro_def->tail = new_line;
	}
	return 1;
}

/**
 * @brief Prints a formatted line to a file.
 *
 * This utility function is used to write lines to the output file with proper
 * formatting, including handling labels and indentation.
 *
 * @param fp The file pointer to write to.
 * @param label The label string, or NULL if no label is present.
 * @param text The rest of the line content.
 */
void print_formatted_line(FILE *fp, const char *label, const char *text)
{
	int label_len;
	int spaces;
	if (label != NULL)
	{
		fprintf(fp, "%s:", label);

		label_len = strlen(label);
		spaces = CODE_COLUMN - (label_len + 1);
		if (spaces < 1) spaces = 1;

		fprintf(fp, "\t%s\n", text);
	}
	else
	{
		fprintf(fp, "%*s%s\n", CODE_COLUMN, "", text);
	}
}


/**
 * @brief The main function for the pre-assembler pass.
 *
 * This function performs two passes over the input assembly file.
 *
 * **First Pass:**
 * - It reads the input file line by line to identify and store all macro definitions.
 * - It validates macro names and checks for re-definitions or invalid syntax.
 * - Errors are collected in the error table.
 *
 * **Second Pass:**
 * - If no errors were found in the first pass, it reads the input file again.
 * - It expands any macro calls by replacing the macro name with its stored content.
 * - All other lines are written to the new `.am` output file without changes.
 *
 * @param input_filename The name of the input `.as` file.
 * @param errortable_ptr A pointer to the error table.
 * @param ec_ptr A pointer to the error counter.
 * @param esize_ptr A pointer to the size of the error table.
 * @param macros_list_head A pointer to the head of the macro list.
 * @return 0 on successful completion, or EXIT on a critical memory or file error.
 */
int pre_assemble(const char *input_filename, struct error *errortable_ptr, int *ec_ptr, int *esize_ptr, MacroDefinition **macros_list_head)
{
	FILE *input_fp = NULL;
	FILE *output_fp = NULL;
	char output_filename[FILENAME_MAX];
	char line[MAX_LINE_LENGTH + 2];
	int line_num = 0;
	int in_macro_definition = 0;
	MacroDefinition *current_macro = NULL;
	char *dot;
	char *original_line_copy_for_content = NULL;
	char *processed_line_for_tokens = NULL;
	char *current_line_ptr;
	char *first_token_candidate;
	char *label_name;
	char *trimmed_line_no_comments;
	char *actual_first_token;
	char *trimmed_content = NULL;
	char *first_char = line;
	int c;

	strncpy(output_filename, input_filename, FILENAME_MAX - 4);
	output_filename[FILENAME_MAX - 4] = '\0';
	dot = strrchr(output_filename, '.');

	if(dot != NULL && strcmp(dot, ".as") == 0)
	{
		strcpy(dot, ".am");
	}
	else
	{
		strcat(output_filename, ".am");
	}

	input_fp = fopen(input_filename, "r");
	if(!input_fp)
	{
		fprintf(stdout, "allocation failed");
		return EXIT;
	}

	/* --- First Pass: Collect Macro Definitions and Check for Errors --- */
	while(fgets(line, sizeof(line), input_fp) != NULL)
	{
		line_num++;
		if(strlen(line) > MAX_LINE_LENGTH + 1)
		{
			if(add_error(&errortable_ptr, ec_ptr, line_num, ": Line exceeds the maximum length of 80 characters.", esize_ptr) == EXIT){goto cleanup_pass1;}
			if (strchr(line, '\n') == NULL && !feof(input_fp))
			{
				while ((c = fgetc(input_fp)) != '\n' && c != EOF);
			}

			continue;
		}
		first_char = line;

		while (*first_char != '\0' && isspace((unsigned char)*first_char))
		{
			first_char++;
		}

		if (*first_char == ';' || *first_char == '\0')
		{
			if (strchr(line, '\n') == NULL && !feof(input_fp))
			{
				while ((c = fgetc(input_fp)) != '\n' && c != EOF);
			}
			continue;
		}

		original_line_copy_for_content = my_strdup(line);
		if(!original_line_copy_for_content)
		{
			fprintf(stdout, "allocation failed");
			goto cleanup_pass1;
		}

		processed_line_for_tokens = my_strdup(line);
		if(!processed_line_for_tokens)
		{
			fprintf(stdout, "allocation failed");
			free(original_line_copy_for_content);
			goto cleanup_pass1;
		}

		current_line_ptr = processed_line_for_tokens;
		first_token_candidate = get_next_token(&current_line_ptr);
		label_name = NULL;
		actual_first_token = first_token_candidate;

		if(first_token_candidate != NULL && strchr(first_token_candidate, ':') != NULL)
		{
			label_name = first_token_candidate;
			actual_first_token = get_next_token(&current_line_ptr);
		}

		trimmed_line_no_comments = trim_whitespace_and_comments(original_line_copy_for_content);

		if(actual_first_token != NULL)
		{
			if(strcmp(actual_first_token, MACRO_START_KEYWORD) == 0)
			{
				char *macro_name_candidate = get_next_token(&current_line_ptr);

				if(macro_name_candidate == NULL || strlen(macro_name_candidate) == 0 || strlen(macro_name_candidate) > MAX_LABEL_LENGTH)
				{
					if(add_error(&errortable_ptr, ec_ptr, line_num, ": Invalid or missing macro name for 'mcro' directive.", esize_ptr) == EXIT){goto cleanup_pass1;}
				}
				else if(!is_valid_macro_name(macro_name_candidate))
				{
					if(add_error(&errortable_ptr, ec_ptr, line_num, ": Macro name contains invalid characters. Must start with a letter and be alphanumeric.", esize_ptr) == EXIT){goto cleanup_pass1;}
				}
				else
				{
					if(is_reserved_word(macro_name_candidate))
					{
						if(add_error(&errortable_ptr, ec_ptr, line_num, ": Macro name cannot be a reserved word (instruction, directive, or register).", esize_ptr) == EXIT){goto cleanup_pass1;}
					}
					else if(find_macro_definition(*macros_list_head, macro_name_candidate) != NULL)
					{
						if(add_error(&errortable_ptr, ec_ptr, line_num, ": Macro with this name already defined (redefinition).", esize_ptr) == EXIT){goto cleanup_pass1;}
					}
					else
					{
						current_macro = (MacroDefinition *)malloc(sizeof(MacroDefinition));
						if(!current_macro)
						{
							fprintf(stdout, "allocation failed");
							goto cleanup_pass1;
						}
						strcpy(current_macro->name, macro_name_candidate);
						current_macro->head = NULL;
						current_macro->tail = NULL;
						current_macro->next = NULL;
						in_macro_definition = 1;
					}
				}
			}
			else if(strcmp(actual_first_token, MACRO_END_KEYWORD) == 0)
			{
				while(isspace((unsigned char)*current_line_ptr))
				{
					current_line_ptr++;
				}
				if(*current_line_ptr != '\0')
				{
					if(add_error(&errortable_ptr, ec_ptr, line_num, ": Unexpected text after 'endmcro'.", esize_ptr) == EXIT){goto cleanup_pass1;}
				}
				if(in_macro_definition == 0)
				{
					if(add_error(&errortable_ptr, ec_ptr, line_num, ": endmcro directive without a preceding mcro definition.", esize_ptr) == EXIT){goto cleanup_pass1;}
				}
				else
				{
					add_macro_definition(macros_list_head, current_macro);
					in_macro_definition = 0;
					current_macro = NULL;
				}
			}
			else if(in_macro_definition == 1)
			{
				if(trimmed_line_no_comments != NULL)
				{
					if(add_macro_line(current_macro, trimmed_line_no_comments) == EXIT){goto cleanup_pass1;}
				}
			}
		}

		free(processed_line_for_tokens);
		processed_line_for_tokens = NULL;
		free(original_line_copy_for_content);
		original_line_copy_for_content = NULL;
	}

	if(in_macro_definition == 1)
	{
		if(add_error(&errortable_ptr, ec_ptr, line_num, ": Unclosed macro definition (missing endmcro).", esize_ptr) == EXIT){goto cleanup_pass1;}
	}
	fclose(input_fp);
	input_fp = NULL;

	if(*ec_ptr > 0)
	{
		return 0;
	}
	else
	{
		/* --- Second Pass: Expand Macros and Write Output --- */
		output_fp = fopen(output_filename, "w");
		if(!output_fp)
		{
			fprintf(stdout, "allocation failed");
			return EXIT;
		}

		input_fp = fopen(input_filename, "r");
		if(!input_fp)
		{
			fprintf(stdout, "allocation failed");
			fclose(output_fp);
			return EXIT;
		}

		line_num = 0;
		in_macro_definition = 0;

		while(fgets(line, sizeof(line), input_fp) != NULL)
		{
			line_num++;
			first_char = line;

			while (*first_char != '\0' && isspace((unsigned char)*first_char))
			{
				first_char++;
			}

			if (*first_char == ';' || *first_char == '\0')
			{
				if (strchr(line, '\n') == NULL && !feof(input_fp))
				{
					while ((c = fgetc(input_fp)) != '\n' && c != EOF);
				}
				continue;
			}

			original_line_copy_for_content = my_strdup(line);
			if(!original_line_copy_for_content)
			{
				fprintf(stdout, "allocation failed");
				goto cleanup_pass2;
			}

			processed_line_for_tokens = my_strdup(line);
			if(!processed_line_for_tokens)
			{
				fprintf(stdout, "allocation failed");
				free(original_line_copy_for_content);
				goto cleanup_pass2;
			}

			current_line_ptr = processed_line_for_tokens;
			first_token_candidate = get_next_token(&current_line_ptr);
			label_name = NULL;
			actual_first_token = first_token_candidate;

			if(first_token_candidate != NULL && strchr(first_token_candidate, ':') != NULL)
			{
				label_name = first_token_candidate;
				actual_first_token = get_next_token(&current_line_ptr);
			}

			if(actual_first_token != NULL)
			{
				if(strcmp(actual_first_token, MACRO_START_KEYWORD) == 0)
				{
					in_macro_definition = 1;
				}
				else if(strcmp(actual_first_token, MACRO_END_KEYWORD) == 0)
				{
					in_macro_definition = 0;
				}
				else if(in_macro_definition == 1)
				{
					/* Do nothing, we are inside a macro definition. */
				}
				else
				{
					MacroDefinition *called_macro = find_macro_definition(*macros_list_head, actual_first_token);

					if(called_macro != NULL)
					{
						MacroLine *ml = called_macro->head;

						if(label_name != NULL)
						{
							trimmed_content = trim_whitespace_and_comments(my_strdup(ml->content));
							fprintf(output_fp, "%s:\t%s\n", label_name, trimmed_content);
							free(trimmed_content);
							trimmed_content = NULL;
							ml = ml->next;
						}

						while(ml != NULL)
						{
							trimmed_content = trim_whitespace_and_comments(my_strdup(ml->content));
							if(!trimmed_content)
							{
								goto cleanup_pass2;
							}
							fprintf(output_fp, "\t%s\n", trimmed_content);
							free(trimmed_content);
							trimmed_content = NULL;
							ml = ml->next;
						}
					}
					else
					{
						if(label_name != NULL)
						{
							print_formatted_line(output_fp, label_name, current_line_ptr);
						}
						else if(original_line_copy_for_content != NULL)
						{
							fputs(original_line_copy_for_content, output_fp);
						}
					}
				}
			}
			else
			{
				if(label_name != NULL)
				{
					fputs(original_line_copy_for_content, output_fp);
				}
			}

			free(processed_line_for_tokens);
			processed_line_for_tokens = NULL;
			free(original_line_copy_for_content);
			original_line_copy_for_content = NULL;
		}

		cleanup_pass2:
			if (input_fp) fclose(input_fp);
			if (output_fp) fclose(output_fp);
			if (processed_line_for_tokens) free(processed_line_for_tokens);
			if (original_line_copy_for_content) free(original_line_copy_for_content);
		return 0;
	}

	cleanup_pass1:
		if (input_fp) fclose(input_fp);
		if (original_line_copy_for_content) free(original_line_copy_for_content);
		if (processed_line_for_tokens) free(processed_line_for_tokens);
		if (current_macro) free_macro_definitions(&current_macro);
		if(macros_list_head) free_macro_definitions(macros_list_head);
	return EXIT;
}
