; Program designed to test error handling of the assembler.
; This file should *not* produce any .ob, .am, .ent, .ext files.
; All output should be error messages to stderr.

; --- Invalid Label Definitions ---
1INVALID_LABEL: mov r0, r1       
mov: add r2, r3                   
MyVeryLongLabelExceedingThirtyOneCharLimit: mov r0, r0 

; --- Invalid Instructions/Operands ---
UNKNOWN_CMD r0, r1              
mov r0 r1                      
add r0, r1, r2                 
sub r0,                      
prn #NotANumber           
mov #LabelAsImmediate, r0   

; --- Invalid Directives ---
.invalid_directive 1, 2        

; --- Duplicate Definitions ---
DuplicateLabel: mov r0, r0
DuplicateLabel: add r0, r0  

; --- Extern/Entry Conflicts ---
ConfkictSym: mov r0, r0
.extern ConfkictSym            

.extern AnotherConflict     
AnotherConflict: mov r0, r0    

WARNING: .entry TMP
cvy:mov r1, #5
stop                         

