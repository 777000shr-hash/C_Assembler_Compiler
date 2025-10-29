#include "assembler.h"

/**
 * @file main.c
 * @brief The main entry point for the assembler program.
 *
 * This file contains the `main` function which orchestrates the entire
 * assembly process. It handles command-line arguments, memory allocation,
 * and calls the `pre_assemble` and `passes` functions to process each
 * input file. It also manages error handling, file output (object,
 * entry, and external files), and proper memory cleanup.
 */

int main(int argc, char *argv[])
{
	/* Declare and initialize variables for counters, sizes, and flags. */
	int i,              /* Loop counter for iterating through command-line arguments. */
		ic,             /* Instruction Counter: keeps track of the current instruction address. */
		dc,             /* Data Counter: keeps track of the current data address. */
		ec,             /* Error Counter: counts the number of errors found. */
		exc,            /* External Counter: counts the number of external symbols. */
		lac,            /* Label Counter: counts the number of labels. */
		argct = argc,   /* Local copy of argument count. */
		isize,          /* Size of the instruction memory table. */
		dsize,          /* Size of the data memory table. */
		esize,          /* Size of the error table. */
		lasize,         /* Size of the label table. */
		exsize,         /* Size of the external table. */
		mcro;           /* Status of the pre-assembly process. */

	char *nametmp, *name = NULL;
	
	/* Pointers to various data structures used throughout the assembly process. */
	struct instructionsMemory *instable = NULL;
	struct labelMemory *labeltable = NULL;
	struct error *errortable = NULL;
	struct dataMemory *datatable = NULL;
	struct external *extable = NULL;
	MacroDefinition *macrostable = NULL;

	/* Loop through each file provided as a command-line argument. */
	for(i = 1; i < argct; i++)
	{
		/* Allocate memory for all required tables for the current file. */
		instable = allocated_memory_table();
		labeltable = allocated_label_table();
		errortable = allocated_error_table();
		datatable = allocated_dataMemory_table();
		extable = allocated_extern_table();
		macrostable = allocated_macro_table();

		/* Check for memory allocation failures and jump to cleanup if any occur. */
		if(instable == NULL || labeltable == NULL || errortable == NULL || datatable == NULL || extable == NULL){goto cleanup;}

		/* Initialize all counters and table sizes for the current file. */
		ic = 0;
		dc = 0;
		ec = 0;
		lac = 0;
		exc = 0;
		isize = MAX_SIZE_MEMORY;
		dsize = MAX_SIZE_MEMORY;
		esize = MAX_SIZE_MEMORY;
		lasize = MAX_SIZE_MEMORY;
		exsize = MAX_SIZE_MEMORY;
		
		/* Allocate memory for temporary and final file names. */
		nametmp = (char *)malloc(strlen(argv[i]) + MAX_LEN_OF_STRING_END);
		name = (char *)malloc(strlen(argv[i]) + MAX_LEN_OF_STRING_END);
		if(name == NULL || nametmp == NULL)
		{
			fprintf(stdout, "allocation failed");
			goto cleanup;
		}
		
		/* Copy and append file extensions to the file names. */
		strcpy(name, argv[i]);
		strcpy(nametmp, argv[i]);
		strcat(name, END_SOURCE_FILE_NAME);

		/*
		 * Pre-assembly pass: handles macros and creates a new file.
		 * 'mcro' holds the status of this pass.
		 */
		mcro = pre_assemble(name, errortable, &ec, &esize, &macrostable);
		if(mcro == EXIT)
		{
			goto cleanup;
		}

		/* Update file name to the pre-assembled output for the next pass. */
		strcpy(name, nametmp);
		strcat(name, END_OF_MACRO_FILE_NAME);

		/*
		 * Main assembly passes (first and second).
		 * Processes instructions, directives, and symbol table management.
		 */
		if(passes(name, instable, labeltable, errortable, extable, datatable, macrostable, &ic, &dc, &ec, &lac, &exc, &isize, &dsize, &esize, &lasize, &exsize) == EXIT){goto cleanup;}

		/* Check if the total memory usage exceeds the maximum allowed size. */
		if((ic + dc) > 156)
		{
			if(add_error(&errortable, &ec, 0, ": the memory is over", &esize) == EXIT){goto cleanup;}
		}

		/* If errors were found, print them and remove the temporary macro file. */
		if(ec > 0)
		{
			strcpy(name, nametmp);
			strcat(name, END_OF_MACRO_FILE_NAME);
			print_error(errortable, ec);
			if(remove(name) != 0)
			{
				fprintf(stdout, "error remove macro file");
			}
		}
		/* If no errors, generate output files. */
		else
		{
			/* Generate `.ext` file if external symbols exist. */
			if(exc > 0)
			{
				strcpy(name, nametmp);
				strcat(name, END_EX_FILE_NAME);
				if(print_extern(name, extable, exc) == EXIT){goto cleanup;}
			}
			
			/* Generate `.ent` file if entry labels exist. */
			if(have_entry(labeltable, &lac))
			{
				strcpy(name, nametmp);
				strcat(name, END_EN_FILE_NAME);
				if(print_entry(name, labeltable, &lac) == EXIT){goto cleanup;}
			}

			/* Generate the `.ob` (object) file. */
			strcpy(name, nametmp);
			strcat(name, END_OBJECT_FILE_NAME);
			if(print_object(name, instable, datatable, ic, dc) == EXIT){goto cleanup;}
		}
		
		/* Free all dynamically allocated memory for the current file. */
		free(instable);
		free(labeltable);
		free(errortable);
		free(extable);
		free(datatable);
		free_macro_definitions(&macrostable);
		free(nametmp);
		free(name);
	}
	
	/* Return success code. */
	return 0;	

	/*
	 * Cleanup block: A `goto` label for centralized memory deallocation
	 * in case of a critical error.
	 */
	cleanup:
		if(instable)free(instable);
		if(labeltable)free(labeltable);
		if(errortable)free(errortable);
		if(extable)free(extable);
		if(datatable)free(datatable);
		if(macrostable)free_macro_definitions(&macrostable);
		if(nametmp)free(nametmp);
		if(name)free(name);
		exit(1); /* Exit with an error code. */
}
