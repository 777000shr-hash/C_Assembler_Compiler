#include "data.h"

/**
 * @brief Allocates memory for the instruction table.
 *
 * This function dynamically allocates an array of `struct instructionsMemory`
 * with a size of `MAX_SIZE_MEMORY`. The memory is initialized to zero using
 * `calloc`. It is a crucial step for storing the machine code generated
 * during the assembly process.
 *
 * @return A pointer to the newly allocated instruction table, or NULL if
 * memory allocation fails.
 */
struct instructionsMemory* allocated_memory_table()
{
	struct instructionsMemory *instable;
	instable = (struct instructionsMemory *)calloc(MAX_SIZE_MEMORY, sizeof(struct instructionsMemory));
	if (instable == NULL)
	{
		fprintf(stdout,"Memory allocation failed");
		return NULL;
	}
	return instable;
}

/**
 * @brief Allocates memory for the label table.
 *
 * This function dynamically allocates an array of `struct labelMemory`
 * to store information about all labels (symbols) defined in the assembly
 * source file. The memory is initialized to zero.
 *
 * @return A pointer to the newly allocated label table, or NULL if
 * memory allocation fails.
 */
struct labelMemory* allocated_label_table()
{
	struct labelMemory *labeltable;
	labeltable = (struct labelMemory *)calloc(MAX_SIZE_MEMORY, sizeof(struct labelMemory));
	if (labeltable == NULL)
	{
		fprintf(stdout,"Memory allocation failed");
		return NULL;
	}
	return labeltable;
}

/**
 * @brief Allocates memory for the error table.
 *
 * This function dynamically allocates an array of `struct error` to
 * store any errors encountered during the assembly process. This table
 * is used to report all errors at the end of the run. The memory is
 * initialized to zero.
 *
 * @return A pointer to the newly allocated error table, or NULL if
 * memory allocation fails.
 */
struct error* allocated_error_table()
{
	struct error *errortable;
	errortable = (struct error*)calloc(MAX_SIZE_MEMORY, sizeof(struct error));
	if (errortable == NULL)
	{
		fprintf(stdout,"Memory allocation failed");
		return NULL;
	}
	return errortable;
}

/**
 * @brief Allocates memory for the external labels table.
 *
 * This function dynamically allocates an array of `struct external`
 * to keep track of labels that are declared as `extern`. This information
 * is later used to generate the `.ext` output file.
 *
 * @return A pointer to the newly allocated external table, or NULL if
 * memory allocation fails.
 */
struct external* allocated_extern_table()
{
	struct external *extable;
	extable = (struct external*)calloc(MAX_SIZE_MEMORY, sizeof(struct external));
	if (extable == NULL)
	{
		fprintf(stdout,"Memory allocation failed");
		return NULL;
	}
	return extable;
}

/**
 * @brief Allocates memory for the data memory table.
 *
 * This function dynamically allocates an array of `struct dataMemory`
 * to store values defined by `.data` and `.string` directives.
 * The memory is initialized to zero.
 *
 * @return A pointer to the newly allocated data table, or NULL if
 * memory allocation fails.
 */
struct dataMemory* allocated_dataMemory_table()
{
	struct dataMemory *datatable;
	datatable = (struct dataMemory*)calloc(MAX_SIZE_MEMORY, sizeof(struct dataMemory));
	if (datatable == NULL)
	{
		fprintf(stdout,"Memory allocation failed");
		return NULL;
	}
	return datatable;
}

/**
 * @brief Adds an error to the error table.
 *
 * This function adds a new error message and its corresponding line number
 * to the error table. If the table is full, it attempts to reallocate
 * more memory to double its size.
 *
 * @param errortable A pointer to a pointer to the error table.
 * @param ec A pointer to the error counter.
 * @param line_number The line number where the error occurred.
 * @param error_message The error message string.
 * @param esize A pointer to the current size of the error table.
 * @return 1 on success, or EXIT on a memory reallocation failure.
 */
int add_error(struct error **errortable, int *ec, int line_number, const char *error_message, int *esize)
{
	struct error *new_errortable;
	if(*ec >= *esize)
	{
		(*esize) = (*esize) * DOUBLE;
		new_errortable = (struct error*)realloc(*errortable, (*esize) * sizeof(struct error));
		if(new_errortable == NULL)
		{
			fprintf(stdout,"Memory allocation failed");
			return EXIT;
		}
		*errortable = new_errortable;
	}
	if(strlen(error_message) > MAX_SIZE_MASAGE){}
	else
	{
		strcpy((*errortable)[*ec].error, error_message);
	}
	(*errortable)[*ec].line = line_number;
	(*ec)++;
	return 1;
}

/**
 * @brief Adds a new label to the label table.
 *
 * This function adds a label's name, type, and memory index to the label table.
 * It first checks if the label already exists using `search_label` to handle
 * re-definitions or special cases like `entry` and `extern`. If the table is
 * full, it reallocates more memory.
 *
 * @param labeltable A pointer to a pointer to the label table.
 * @param lac A pointer to the label counter.
 * @param str The name of the label.
 * @param lasize A pointer to the current size of the label table.
 * @param type The type of the label (INSTRUCTION, DIRECTIVE, EXTERN, ENTRY).
 * @param ic The current instruction counter.
 * @param dc The current data counter.
 * @return 1 on success, or EXIT on a memory reallocation failure.
 */
int add_label(struct labelMemory **labeltable, int *lac, char str[], int *lasize, int type, int ic, int dc)
{
	struct labelMemory *new_labeltable;
	if(*lac >= *lasize)
	{
		(*lasize) = (*lasize) * DOUBLE;
		new_labeltable = (struct labelMemory*)realloc(*labeltable, (*lasize) * sizeof(struct labelMemory));
		if(new_labeltable == NULL)
		{
			fprintf(stdout,"Memory allocation failed");
			return EXIT;
		}
		*labeltable = new_labeltable;
	}
	if(!(search_label(*labeltable, lac, str, lasize, type, ic, dc)))
	{
		strcpy((*labeltable)[*lac].name, str);
		if(type == INSTRUCTION)
		{
			(*labeltable)[*lac].type = (type);
			(*labeltable)[*lac].index = (ic);
		}
		else if(type == DIRECTIVE)
		{
			(*labeltable)[*lac].type = (type);
			(*labeltable)[*lac].index = (dc);
		}
		if(type == EXTERN)
		{
			(*labeltable)[*lac].en = EXTERN;
		}
		if(type == ENTRY)
		{
			(*labeltable)[*lac].en = ENTRY;
		}
		(*lac)++;
	}
	return 1;
}

/**
 * @brief Adds a data value to the data memory table.
 *
 * This function stores an integer value into the data memory table. The value
 * typically comes from a `.data` or `.string` directive. It handles
 * reallocation if the table reaches its capacity.
 *
 * @param datatable A pointer to a pointer to the data table.
 * @param val The integer value to be stored.
 * @param dc A pointer to the data counter.
 * @param dsize A pointer to the current size of the data table.
 * @return 1 on success, or EXIT on a memory reallocation failure.
 */
int add_data(struct dataMemory **datatable, int val,int *dc,int *dsize)
{
	struct dataMemory *new_datatable;
	if(*dc >= *dsize)
	{
		(*dsize) = (*dsize) * DOUBLE;
		new_datatable = (struct dataMemory*)realloc(*datatable, (*dsize) * sizeof(struct dataMemory));
		if(new_datatable == NULL)
		{
			fprintf(stdout,"Memory allocation failed");
			return EXIT;
		}
		*datatable = new_datatable;
	}
	(*datatable)[*dc].address = val;
	(*dc)++;
	return 1;
}

/**
 * @brief Adds an instruction or operand word to the instruction table.
 *
 * This is a versatile function that adds a new machine code word to the
 * instruction table. It handles three types of records: command words,
 * register words, and address words, each with its specific data structure.
 * It also manages memory reallocation if the table fills up.
 *
 * @param instable A pointer to a pointer to the instruction table.
 * @param ic A pointer to the instruction counter.
 * @param isize A pointer to the current size of the instruction table.
 * @param type The type of record to add (RECORD_TYPE_COMMAND, etc.).
 * @param val The opcode value for command records.
 * @param operand1 The first operand's value (or register number).
 * @param operand2 The second operand's value (or register number).
 * @param are The relocation type (Absolute, Relocatable, External).
 * @param address The address or immediate value.
 * @return 1 on success, or EXIT on a memory reallocation failure.
 */
int add_ins(struct instructionsMemory **instable, int *ic, int *isize, enum RecordType type, int val, int operand1, int operand2, int are, int address)
{
	struct instructionsMemory *new_instable;
	if(*ic >= *isize)
	{
		(*isize) = (*isize) * DOUBLE;
		new_instable = (struct instructionsMemory*)realloc(*instable, (*isize) * sizeof(struct instructionsMemory));
		if(new_instable == NULL)
		{
			fprintf(stdout,"Memory allocation failed");
			return EXIT;
		}
		*instable = new_instable;
	}
	if(RECORD_TYPE_COMMAND == type)
	{
		(*instable)[(*ic)].type = RECORD_TYPE_COMMAND;
		(*instable)[(*ic)].data.command.opcode = val;
		(*instable)[(*ic)].data.command.operand1 = operand1;
		(*instable)[(*ic)].data.command.operand2 = operand2;
		(*instable)[(*ic)].data.command.ARE = 0;
	}
	else if(RECORD_TYPE_REGISTER == type)
	{
		(*instable)[(*ic)].type = RECORD_TYPE_REGISTER;
		(*instable)[(*ic)].data.reg.ARE = are;
		(*instable)[(*ic)].data.reg.operand1 = operand1;
		(*instable)[(*ic)].data.reg.operand2 = operand2;
	}
	else if(RECORD_TYPE_ADDRESS == type)
	{
		(*instable)[(*ic)].type = RECORD_TYPE_ADDRESS;
		(*instable)[(*ic)].data.addr.ARE = are;
		(*instable)[(*ic)].data.addr.address = address;
	}
	(*ic)++;
	return 1;
}

/**
 * @brief Adds an external label to the external table.
 *
 * This function adds a label's name and the memory address where it is used
 * to the external table. This is part of the process of generating the `.ext` file.
 *
 * @param str The name of the external label.
 * @param extable A pointer to a pointer to the external table.
 * @param exsize A pointer to the current size of the external table.
 * @param exc A pointer to the external counter.
 * @param index The memory address where the external label is referenced.
 * @return 1 on success, or EXIT on a memory reallocation failure.
 */
int add_extern(char str[], struct external **extable, int *exsize, int *exc, int index)
{
	struct external *new_extable;
	if(*exc >= *exsize)
	{
		(*exsize) = (*exsize) * DOUBLE;
		new_extable = (struct external*)realloc(*extable, (*exsize) * sizeof(struct external));
		if(new_extable == NULL)
		{
			fprintf(stdout,"Memory allocation failed");
			return EXIT;
		}
		*extable = new_extable;
	}
	strcpy((*extable)[*exc].name, str);
	(*extable)[*exc].index = index;
	(*exc)++;
	return 1;
}

/**
 * @brief Prints all collected errors to standard output.
 *
 * This function iterates through the error table and prints each error message
 * along with its corresponding line number.
 *
 * @param errortable A pointer to the error table.
 * @param ec The number of errors in the table.
 */
void print_error(struct error *errortable, int ec)
{
	int i = 0;
	for(; i < ec; i++)
	{
		if(errortable[i].error != NULL)
		{
			fprintf(stdout, "%d%s\n", (errortable[i].line + 1), errortable[i].error);
		}
	}
}

/**
 * @brief Prints the entry labels to a file.
 *
 * This function generates the `.ent` output file. It iterates through the
 * label table and prints the name and final memory address of any label
 * that is marked as `entry`. The address is converted to a special
 * base-4 format.
 *
 * @param name The name of the output file.
 * @param labeltable A pointer to the label table.
 * @param lac A pointer to the label counter.
 * @return 1 on success, or EXIT on a file or memory error.
 */
int print_entry(char name[], struct labelMemory *labeltable, int *lac)
{
	FILE *fileptr;
	int i = 0;
	char *word;
	fileptr = fopen(name, "wb");
	if (fileptr == NULL)
	{
		fprintf(stdout,"error opening file!\n");
		return EXIT;
	}
	for(; i < (*lac); i++)
	{
		if(labeltable[i].en == ENTRY)
		{
			word = int_to_special_base4(labeltable[i].index, NUM_4);
			if(word == NULL){goto clean;}
			fprintf(fileptr, "%s	%s\n", labeltable[i].name, word);
			free(word);
		}
	}
	fclose(fileptr);
	return 1;
	clean:
		fclose(fileptr);
		free(word);
		return EXIT;
}

/**
 * @brief Prints the external labels to a file.
 *
 * This function generates the `.ext` output file. It iterates through the
 * external table and prints the name of each external label along with the
 * memory address where it was referenced. The address is converted to a
 * special base-4 format.
 *
 * @param name The name of the output file.
 * @param extable A pointer to the external table.
 * @param exc The number of external labels.
 * @return 1 on success, or EXIT on a file or memory error.
 */
int print_extern(char name[], struct external *extable, int exc)
{
	FILE *fileptr;
	int i = 0;
	char *word;
	fileptr = fopen(name, "wb");
	if (fileptr == NULL)
	{
		printf("error opening file!\n");
		return EXIT;
	}
	for(; i < exc; i++)
	{
		word = int_to_special_base4(extable[i].index, NUM_4);
		if(word == NULL){goto clean;}
		fprintf(fileptr, "%s	%s\n", extable[i].name, word);
		free(word);
	}
	fclose(fileptr);
	return 1;
	clean:
		fclose(fileptr);
		free(word);
		return EXIT;
}

/**
 * @brief Prints the object file with machine code.
 *
 * This is the main function for generating the `.ob` output file. It prints
 * the final instruction and data counter values, followed by the machine code
 * for all instructions and data. The addresses and code are converted to a
 * special base-4 format before being written to the file.
 *
 * @param name The name of the output file.
 * @param instable A pointer to the instruction table.
 * @param datatable A pointer to the data table.
 * @param ic The final instruction counter.
 * @param dc The final data counter.
 * @return 1 on success, or EXIT on a file or memory error.
 */
int print_object(char name[], struct instructionsMemory *instable, struct dataMemory *datatable, int ic, int dc)
{
	FILE *fileptr;
	int i = 0, j = 0, c = 0, d = 0;
	char *index, *word1, *word2, *word3, *word4;
	fileptr = fopen(name, "wb");
	if (fileptr == NULL)
	{
		fprintf(stdout,"error opening file!\n");
		return EXIT;
	}
	word1 = int_to_special_base4(ic, NUM_8);
	word2 = int_to_special_base4(dc, NUM_8);
	if(word1 == NULL || word2 == NULL){goto cn;}
	while(word1[c] == 'a')
	{
		c++;
	}
	while(word2[d] == 'a')
	{
		d++;
	}
	word3 = (char *)malloc(strlen(word1) - c + 1);
	word4 = (char *)malloc(strlen(word2) - d + 1);
	if (word3 == NULL || word4 == NULL)
	{
		fprintf(stdout,"error allocate!\n");
		goto cn;
	}
	strcpy(word3, word1 + c);
	strcpy(word4, word2 + d);
	fprintf(fileptr, " %s %s\n", word3, word4);
	free(word1);
	free(word2);
	free(word3);
	free(word4);
	for(; i < ic; i++)
	{
		index = int_to_special_base4((i + MEMORY_START), NUM_4);
		if (index == NULL){goto cn;}
		if(instable[i].type == RECORD_TYPE_COMMAND)
		{
			word1 = int_to_special_base4(instable[i].data.command.opcode, NUM_2);
			word2 = int_to_special_base4(instable[i].data.command.operand1, NUM_1);
			word3 = int_to_special_base4(instable[i].data.command.operand2, NUM_1);
			word4 = int_to_special_base4(instable[i].data.command.ARE, NUM_1);
			if(word1 == NULL || word2 == NULL || word3 == NULL || word4 == NULL){goto cn;}
			fprintf(fileptr, "%s	%s%s%s%s\n", index, word1, word2, word3, word4);
			free(word1);
			free(word2);
			free(word3);
			free(word4);
		}
		else if(instable[i].type == RECORD_TYPE_REGISTER)
		{
			word1 = int_to_special_base4(instable[i].data.reg.operand1, NUM_2);
			word2 = int_to_special_base4(instable[i].data.reg.operand2, NUM_2);
			word3 = int_to_special_base4(instable[i].data.reg.ARE, NUM_1);
			if(word1 == NULL || word2 == NULL || word3 == NULL){goto cn;}
			fprintf(fileptr, "%s	%s%s%s\n", index, word1, word2, word3);
			free(word1);
			free(word2);
			free(word3);
		}
		else if(instable[i].type == RECORD_TYPE_ADDRESS)
		{
			word1 = int_to_special_base4(instable[i].data.addr.address, NUM_4);
			word2 = int_to_special_base4(instable[i].data.addr.ARE, NUM_1);
			if (word1 == NULL || word2 == NULL){goto cn;}
			fprintf(fileptr, "%s	%s%s\n", index, word1, word2);
			free(word1);
			free(word2);
		}
		free(index);

	}
	for(; j < dc; j++)
	{
		index = int_to_special_base4((i + MEMORY_START), NUM_4);
		word1 = int_to_special_base4(datatable[j].address, NUM_5);
		if (word1 == NULL || index == NULL){goto cn;}
		fprintf(fileptr, "%s	%s\n", index, word1);
		i++;
		free(index);
		free(word1);
	}
	fclose(fileptr);
	return 1;

	cn:
	if(word1)free(word1);
	if(word2)free(word2);
	if(word3)free(word3);
	if(word4)free(word4);
	if(index)free(index);
	fclose(fileptr);
	return EXIT;
}

/**
 * @brief Checks if there are any entry labels in the label table.
 *
 * This utility function is used to determine if an `.ent` file needs to be
 * generated. It iterates through the label table and returns true if it
 * finds any label marked as `entry`.
 *
 * @param labeltable A pointer to the label table.
 * @param lac A pointer to the label counter.
 * @return 1 if an entry label exists, 0 otherwise.
 */
int have_entry(struct labelMemory *labeltable, int *lac)
{
	int i = 0;
	for(; i < (*lac); i++)
	{
		if(labeltable[i].en == ENTRY)
		{
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Searches for a label and updates its properties if found.
 *
 * This function is used to find a label in the table by its name. If found, it
 * updates its properties, such as setting it as `extern` or `entry`, or
 * updating its type and index if it was previously undefined.
 *
 * @param labeltable A pointer to the label table.
 * @param lac A pointer to the label counter.
 * @param str The name of the label to search for.
 * @param lasize A pointer to the current size of the label table.
 * @param type The type to update the label with (EXTERN, ENTRY, etc.).
 * @param ic The current instruction counter.
 * @param dc The current data counter.
 * @return 1 if the label is found and updated, 0 otherwise.
 */
int search_label(struct labelMemory *labeltable, int *lac, char *str, int *lasize, int type, int ic, int dc)
{
	int i = 0;
	for(; i < *lac; i++)
	{
		if(strcmp(labeltable[i].name, str) == 0)
		{
			if(type == EXTERN)
			{
				labeltable[i].en = EXTERN;
				return 1;
			}
			else if(type == ENTRY)
			{
				labeltable[i].en = ENTRY;
				return 1;
			}
			else if(type == INSTRUCTION || type == DIRECTIVE)
			{
				if(labeltable[i].en == ENTRY)
				{
					labeltable[i].type = (type);
					if(type == DIRECTIVE)
					{
						labeltable[i].index = (dc);
						return 1;
					}
					else if(type == INSTRUCTION)
					{
						labeltable[i].index = (ic);
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
