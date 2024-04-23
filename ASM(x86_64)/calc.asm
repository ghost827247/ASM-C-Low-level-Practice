section .data
	first db "Enter First Number: "
	f_len equ $ -first

	second db "Enter Second Number: ", 0
	s_len equ $ -second

	operator db "1 For +, 2 For -, 3 For *, 4 For /: "
	op_len equ $ -operator

	sum_p db "Sum ="
	sum_len equ $ -sum_p

section .bss
	; Buffers To hold Inputted Numbers, can hold 1 byte (8 bits)
	num1 resb 2
	num2 resb 2
	sum resb 2

	op resb 2

section .text
	GLOBAL _start

_start:
	; Use SYS_WRITE system call To Print For input Prompt
	mov eax, 4
	mov ebx, 1
	mov ecx, first
	mov edx, f_len
	int 0x80 ; Int 0x80 Means system interrupt to swap to kernel space and allow kernel to use system calls
		; EAX Holds Syscall Number 4=SYS_WRITE
		; EBX Holds File Descriptor Number, 1 = STDIN
		; ECX Holds Pointer to string to Print
		; EDX Holds Number Of bytes To write, we get the Amount From the f_len equ $ -first

	; Use SYS_READ system call to Read First Number From Standard input
	mov eax, 3
	mov ebx, 0
	mov ecx, num1
	mov edx, 2
	int 0x80
		; EAX holds Syscall Number For the kernel to use to look up, 3= SYS_READ
		; EBX Holds File Descriptor Number, 0 = STDIN
		; ECX holds pointer to Memory address To save to
		; EDX Holds Amount Of Bytes To Read 

	; Use SYS_WRITE system call To Print For input Prompt
	mov eax, 4
	mov ebx, 1
	mov ecx, second
	mov edx, s_len
	int 0x80

	;
	mov eax, 3
	mov ebx, 0
	mov ecx, num2
	mov edx, 2
	int 0x80

	; Use SYS_WRITE system call To Print For Operator Prompt
	mov eax, 4
	mov ebx, 1
	mov ecx, operator
	mov edx, op_len
	int 0x80

	;  Use SYS_READ system call to Read Operator From Standard input
	mov eax, 3
	mov ebx, 0
	mov ecx, op
	mov edx, 2
	int 0x80

	; Convert Chosen Input Number From Ascii To Numeric so we can compare it and jmp to chosen Operation
	mov bh, [op] ; Move Value Stored In OP ([] Means De referecnce, like C's * Char)
	sub bh, '0' 
		; Lets Say User entered Number 2 For addtion, Ascii Code for '0' Is 48
		; Ascii Code for '2' Is 50
		; Subtract 50 From 48 Which Gives Us 2, Easy way to convert single digits

	; Call Convert Numbers Function Then Return Back to Line Afterwards
	call convert_nums

	; If bh = 1 Jump To Addition
	cmp bh, 1
	je addition

	; If bh = 2 Jump To Subtraction
	cmp bh, 2
	je subtraction

	; If bh = 3 Jump To Multipication
	cmp bh, 3
	je multplication

	; If bh = 4 Jump To Division
	cmp bh, 4
	je division

	; TODO Add Check For If Number >4 If so jump Back to Operator Input
	






convert_nums:	
	; Move num1 and num2 into al and bl Registers
	mov al, [num1]
	mov bl, [num2]

	; Subtract 48-n Where N = inputted Numbers to turn it into numeric
	sub al, '0'
	sub bl, '0'

	; Move back into memory After conversion For Later, I think This Is Useless Tho as im using al and bl for the addtion anyway
	mov [num1], al
	mov [num2], bl

	; Return Back To cmp operation
	ret

addition:
	; Add Inputted Num1 and Num2, sum gets saved into AL
	add al, bl

	; Convert Sum From Numeric to Ascii For Printing
	add al, '0'
	; Save Sum Back into SUM location
	mov [sum], al

	; Jump To Print Sum
	jmp print_sum


subtraction:
	; Sub num1 From Num2, Sum Gets Saved Into AL
	sub al, bl

	; Convert Sum Back Into ASCII
	add al, '0'
	; Save SUM into sum memory location
	mov [sum], al

	jmp print_sum;

multplication:
	; Multiply al * bl, SUM gets Saved into 16 bit registers AX
	mul bl

	; Comvert SUM into ASCII
	add ax, '0'

	; Save To SUM location
	mov [sum], ax

	jmp print_sum

division:
	; Divide al / bl, Sum Gets Saved Into AX
	div bl

	; Convert To ASCII
	add ax, '0'
	; Save To SUM location
	mov [sum], ax

	jmp print_sum




print_sum:
	; Use SYS_WRITE To Print Out SUM
	mov eax, 4
	mov ebx, 1
	mov ecx, sum
	mov edx, 2
	int 0x80

	jmp exit



exit:
	; Use SYS_EXIT to exit Program
	mov eax, 1
	xor ebx, ebx ;  Clear EBX registers to 0, Just Like in C, 0 = Exiited With No Problems
	int 0x80
