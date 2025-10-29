#include "file.h"

/**
 * @brief Reads a text file line by line into a dynamic array of strings.
 *
 * This function reads a file, handles potential line length errors, and stores
 * each line in a dynamically allocated array. The array is resized as needed
 * to accommodate the file's content.
 *
 * @param name The name of the file to be read.
 * @param out_num_line A pointer to an integer where the number of lines read
 * will be stored.
 * @param errortable A pointer to the error table structure.
 * @param ec A pointer to the error counter.
 * @param esize A pointer to the size of the error table.
 * @return A dynamically allocated array of strings (char**) containing the
 * file's content, or NULL if an error occurred or the file is empty.
 */
char ** read_file(const char *name, int *out_num_line, struct error *errortable, int *ec, int *esize)
{
	FILE *file_ptr = NULL;
	char **lines_array = NULL;
	char temp_line_buffer[MAX_SIZE_CHAR];
	char *current_line_ptr = NULL;
	int c ,i;
	int num_lines = 0;
	int current_capacity = MAX_SIZE_MEMORY;
	char **temp_ptr;
	int current_file_line = 1;
	size_t len;
	
	/* Initialize the number of lines to 0 */
	*out_num_line = 0;

	/* Allocate initial memory for the array of line pointers */
	lines_array = (char **)malloc(current_capacity * sizeof(char *));
	if (lines_array == NULL)
	{
		fprintf(stdout, "Failed to allocate initial memory for lines_array");
		return NULL;
	}

	/* Open the file for reading */
	file_ptr = fopen(name, "r");
	if (file_ptr == NULL)
	{
		/* Add an error if the file cannot be opened */
		add_error(&errortable, ec, 0, ": error opening file", esize);
		free(lines_array);
		return NULL;
	}

	/* Read the file line by line */
	while (fgets(temp_line_buffer, MAX_SIZE_CHAR, file_ptr) != NULL)
	{
		len = strlen(temp_line_buffer);

		/* Check for lines that are longer than the buffer size (MAX_SIZE_CHAR) */
		if (len > 0 && temp_line_buffer[len - 1] != '\n' && !feof(file_ptr))
		{
			/* Add an error and clear the rest of the line from the stream */
			add_error(&errortable, ec, num_lines, ": line is longer than 80 characters", esize);
			while ((c = fgetc(file_ptr)) != '\n' && c != EOF);
			
			/* Allocate an empty string to mark the line as an error */
			current_line_ptr = (char *)malloc(1 * sizeof(char));
			if (current_line_ptr == NULL)
			{
				fprintf(stdout,"Failed to allocate memory for empty line");
				for (i = 0; i < num_lines; i++)
				{
					free(lines_array[i]);
				}
				free(lines_array);
				fclose(file_ptr);
				return NULL;
			}
			current_line_ptr[0] = '\0';
		}
		else
		{
			/* Remove the newline character if present */
			if (len > 0 && temp_line_buffer[len - 1] == '\n')
			{
				temp_line_buffer[len - 1] = '\0';
				len--;
			}
			
			/* Allocate memory for the line and copy the content */
			current_line_ptr = (char *)malloc((len + 1) * sizeof(char));
			if (current_line_ptr == NULL)
			{
				fprintf(stdout,"Failed to allocate memory for line");
				for (i = 0; i < num_lines; i++)
				{
					free(lines_array[i]);
				}
				free(lines_array);
				fclose(file_ptr);
				return NULL;
			}
			strcpy(current_line_ptr, temp_line_buffer);
			
			/* Resize the array of line pointers if capacity is exceeded */
			if (num_lines >= current_capacity)
			{
				current_capacity *= 2;
				temp_ptr = (char **)realloc(lines_array, current_capacity * sizeof(char *));
				if (temp_ptr == NULL)
				{
					fprintf(stdout,"Failed to reallocate memory for lines_array");
					for (i = 0; i < num_lines; i++)
					{
						free(lines_array[i]);
					}
					free(lines_array);
					fclose(file_ptr);
					free(current_line_ptr);
					return NULL;
				}
				lines_array = temp_ptr;
			}
		}

		/* Add the current line pointer to the array */
		lines_array[num_lines] = current_line_ptr;
		num_lines++;
		current_file_line++;
	}

	fclose(file_ptr);

	/* If no lines were read, free the array and return NULL */
	if (num_lines == 0)
	{
		free(lines_array);
		*out_num_line = 0;
		return NULL;
	}
	else
	{
		/* Reallocate the array to its final size to save memory */
		temp_ptr = (char **)realloc(lines_array, num_lines * sizeof(char *));
		if (temp_ptr == NULL)
		{
			/* If realloc fails, return the current array and its size */
			*out_num_line = num_lines;
			return lines_array;
		}
		lines_array = temp_ptr;
	}

	/* Store the number of lines and return the array */
	*out_num_line = num_lines;
	return lines_array;
}
