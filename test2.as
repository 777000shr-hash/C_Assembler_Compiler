; This is a simple assembly program for Maman 14.
; It demonstrates basic instructions, data, and a macro.

; Declare MAIN and DATA_VAL as entry points.
.entry MAIN
.entry DATA

; Define a simple macro named MY_MACRO
mcro MY_MACRO
  inc r0
  add #10, r1
mcroend


; Group 1 Instructions (two operands)
MAIN:	mov #5, r0        
	mov r0, r1        
	add #20, r1       
	sub r1, r0       

  
; Group 2 Instructions (one operand)
	lea DATA, r2  
	prn r2            
	clr r1            
	not r0            

	inc MYDATA       
	dec MYDATA       

	MY_MACRO          
	prn r0            

; Group 3 Instruction (no operands)
	jmp ENDPROGRAM   

DATA:	.data 100, -50, 0, 123
MYSTRING: .string "Hello World!" 
MYDATA:	.data 25 

ENDPROGRAM:	stop

; This is another comment at the end of the file

