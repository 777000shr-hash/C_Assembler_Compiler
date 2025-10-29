assembler: assembler.o file.o directive.o instruction.o second_pass.o data.o code.o pre_assembler.o
	gcc -Wall -ansi -pedantic -g assembler.o file.o directive.o instruction.o second_pass.o data.o code.o pre_assembler.o -o assembler
assembler.o: assembler.c assembler.h code.h
	gcc -c -Wall -ansi -pedantic -g assembler.c -o assembler.o
file.o: file.c file.h
	gcc -c -Wall -ansi -pedantic -g file.c -o file.o
code.o: code.c code.h
	gcc -c -Wall -ansi -pedantic -g code.c -o code.o
directive.o: directive.c directive.h code.h
	gcc -c -Wall -ansi -pedantic -g directive.c -o directive.o
instruction.o: instruction.c instruction.h 
	gcc -c -Wall -ansi -pedantic -g instruction.c -o instruction.o
second_pass.o: second_pass.c second_pass.h code.h
	gcc -c -Wall -ansi -pedantic -g second_pass.c -o second_pass.o
data.o: data.c data.h 
	gcc -c -Wall -ansi -pedantic -g data.c -o data.o
pre_assembler.o: pre_assembler.c pre_assembler.h
	gcc -c -Wall -ansi -pedantic -g pre_assembler.c -o pre_assembler.o

