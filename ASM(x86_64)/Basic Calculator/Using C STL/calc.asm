section .data
	first_p db "[*] Enter First Number: ", 0                               ; First Prompt For Number Input
	first_l equ $ -first_p                                                 ; Get Amount Of Bytes For Printing (Think Its Not Needed For PrintF tho)
 
	second_p db "[*] Enter Second Number: ", 0                             ; Second Prompt For Number Input
	second_l equ $ -second_p

	op_str db "[*] Enter Operation Choice: ", 0                            ; Operator Input Prompt (+,-,*,/)
	op_l equ $ -op_str

	sum_str db "%d + %d = %d",10,  0                                       ; Place Holders For Where The Numbers Will be Added To (%d For digits)


	choice db "[*] Would You Like To Do Another Computation(1 = Y) : ", 0         ; Ask User If He Would Like To Restart The Program
	choice_l equ $ -choice

	scanf_format db "%d", 0                                                ; Format Specifyer For ScanF So it Knows To Save the Input as numeric, means we dont have To Bother With Conversion

	wall db "==========================", 10, 0


section .bss
	num1 resd 1                       ; Buffer For First Inputted Number, Holds a Double Word (4 Bytes, 32 Bits)
	num2 resd 1                       ; Buffer For Second Inputted Number, Holds a Double Word (4 Bytes, 32 Bits)
	sum resd 1                        ; Buffer For Sum After We do the computations Inputted Number, Holds a Double Word (4 Bytes, 32 Bits)
	op resd 1                         ; Buffer For Chosen Operation, Holds a Double Word (4 Bytes, 32 Bits)

	chc resb 2                        ; Buffer For Holding Restart program Choice (Holds 2 Bytes, 16 bits, 1 For the Null Terminator)

section .text
	GLOBAL _start                     ; Entry point To Program For Compiler/linker
	extern printf                     ; Telling it to link external printF Function
	extern scanf                      ; Telling It to link external scanF function

_start:
	push wall
	call printf
	add esp, 4


	push first_p                      ; Push First Input Prompt Onto Stack For PrintF to Grab
	call printf                       ; Call PrintF For printing to STDOUT
	add esp, 4                        ; Make Stack pointer point Back to it was before we used the push operation, ESP goes up by 4 bytes each push/pop, so if we push 2 items onto stack we would add esp, 8

									  ; ========SCANF========
	lea eax, [num1]                   ; Load Memory Address Of Num1 into Eax, ScanF requires a pointer to where to save input
	push eax                          ; Push memory Address onto Stack
	push DWORD scanf_format           ; Push ScanF Specifiyer onto Stack so it knows to save to numeric digit
	call scanf                        ; call ScanF for Reading From STDIN
	add esp, 8                        ; Make Stack pointer point Back to it was before we used the push operation

	push second_p                     ; Push Second Input Prompt Onto Stack
	call printf                       
	add esp, 4                        

	lea eax, [num2]                   ; Read SCANF above
	push eax
	push DWORD scanf_format
	call scanf
	add esp, 8

	push op_str                       ; Push Chose operation string onto stack
	call printf                       ; Call Scanf
	add esp, 4

	lea eax, [op]                     ; READ SCANF Above
	push eax
	push DWORD scanf_format
	call scanf
	add esp, 8

	mov al, [op]                      ; Move Operation Choice Into AL, Only Need to use lower 8 bits as the number will be between 1-4, AL holds upto 255, so AX, EAX is overkill

	cmp al, 1                         ; If OP = 1, User Chose Addition
	je addition                       ; Jump To addition

	cmp al, 2                         ; If OP = 2, User Chose Subtraction
	je subtraction                    ; Jump To Subtraction

	cmp al, 3                         ; If OP = 3, User Wants Multiplication
	je Multiplication

	cmp al, 4                         ; If OP = 4, User Wants Division
	je division
	jg error                          ; Jump To Error Handle Function If User Entered Higher then 4






addition:                     ; Addition Function
	mov eax, [num1]                   ; Move num1 Into EAX (Using 32 Bit registers as we want the user to be able to do big numbers)
	add eax, [num2]                   ; Add Num1 + Num2, Sum Gets Saved Back Into EAX

	mov [sum], eax                    ; Move Sum Back Into Sum Buffer

	jmp print_sum                     ; Jump To Print_Sum


subtraction:                  ; Subtract Function
	mov eax, [num1]                   ; Move num1 Into EAX (Using 32 Bit registers as we want the user to be able to do big numbers)
	sub eax, [num2] 

	mov [sum], eax                    ; Move Sum Back Into Sum Buffer

	jmp print_sum                     ; Jump To Print_Sum
 

Multiplication:              ; Multiplication Function
	mov eax, [num1]                   ; Really Need To Turn This Into a Function 
	mov ebx, [num2]
	mul ebx                           ; Multilply EAX * EBX, Result Stored In EAX
	mov [sum], eax                    ; Move Result Into SUM

	jmp print_sum                     ; Jump Into Print_sum



division:
	mov eax, [num1]                   ; Mov Num1 Into EAX
	xor edx, edx                      ; Clear EDX as Remainder Gets Saved into It
	mov ebx, [num2]                   ; Mov Num2 Into EBX
	div ebx                           ; Divide Eax / EBX, Result Goes into EAX, Remainder Into EDX
	mov [sum], eax                    ; Move The Result Into Sum

	jmp print_sum                     ; Jump Print Sum



print_sum:                    
	push DWORD [sum]                  ; Push Sum onto The stack we User DWORD as Double Words Are 4 bytes/ 32 bits
	                                  ; %d For PrintF requires a 32 bit integer, so even if the sum is 1 byte it still requires a 32 bit space to be pushed
	push DWORD [num2]                 ; Push Num2 Onto Stack, Read Line Starting With %d
	push DWORD [num1]                 ; Push Num1 Onto The Stack
	push sum_str                      ; Push The String which has the placeholders in it onto stack
	call printf                       ; Call PrintF
	add esp, 16
	                                  ; When using printF in ASM we must push the arguments in reverse onto the stack For it To work properly
	


 ; THIS BIT USES PURE ASM FOR WRITING TO STDOUT AND READING FROM STDIN, I PUT IT IN JUST FOR COMPARING USING EXTERNAL FUNCTIONS AND DOING IT ALL OUR SELF 

	mov eax, 4                        ; Mov 4 Into EAX, 4 = SYS_WRITE in the sys_call Table
	mov ebx, 1                        ; Mov 1 into EBX, 1 = STDOUT File Descriptor
	mov ecx, choice                   ; Mov a the pointer poiting to the string to write
	mov edx, choice_l                 ; Mov The value Holding How many Bytes To write out, 
	                                  ; If we had string with 10 bytes (10 characters) But We moved 8 into EDX, then only 8 Characters Would Get Printed
	int 0x80                          ; Call Interptt To Tell CPU to switch to kernel Mode So we can execute SYS_CALL

	mov eax, 3                        ; Mov 3 Into EAX, 3 = SYS_READ
	mov ebx, 0                        ; Mov 1 Into EBX, 1 = STDIN (Standard Input)
	mov ecx, chc                      ; Move Memory Address Of Location To save input too
	mov edx, 2                        ; Only Read 2 bytes, 1 for character, 2 for character + \0 (i Think this Might be Wrong)
	int 0x80                          ; Call Kernel

	mov al, [chc]                     ; Move Value inside choice (chc) address into al (only Using 8 bit register as number will only be 1 or 2)

	sub al, '0'                       ; Convert Input Into Numeric For Comparing, Ascii Code for '0' = 48, (THIS EXAMPLE THE USER ENTERED: 1)
	                                  ; Since the 1 Is a char it is saved in memory as ascii Code, 1 = 49
	                                  ; So If we Sub 49 - 48 We Get 1, So We Succesfully Converted Into Numeric

	cmp al, 1                         ; Compare User Input To 1
	je _start                         ; Jump To Start If User Wants To Do Another Operation
	jg exit                           ; Jump To Exit If User Wants To exit (Well No Shit)



error:
	



exit:
	mov eax, 1                        ; Mov 1 int EAX, 1 = SYS_EXIT
	xor ebx, ebx                      ; Clear EBX register, as The Exit Code Uses EBX to know if exitted Succesfully, 1 = Problem, 0 = No Problems
	int 0x80                          ; Call Kernel