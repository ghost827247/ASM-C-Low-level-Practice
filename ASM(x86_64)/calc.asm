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
		; EAX Holds Syscall Number 1=STDOUT
		; EBX Holds File Descriptor Numbers
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
		; Subtract 48 From 50 Which Gives Us 2, Easy way to convert single digits

	call convert_nums

	cmp bh, 1
	je addition

	cmp bh, 2
	je subtraction

	cmp bh, 3
	je multplication

	cmp bh, 4
	je division


	






convert_nums:
	mov al, [num1]
	mov bl, [num2]

	sub al, '0'
	sub bl, '0'

	mov [num1], al
	mov [num2], bl

	ret

addition:
	add al, bl

	add al, '0'
	mov [sum], al

	jmp print_sum


subtraction:
	sub al, bl
	add al, '0'
	mov [sum], al

	jmp print_sum;

multplication:
	mul bl

	add ax, '0'

	mov [sum], ax

	jmp print_sum

division:
	div bl
	add ax, '0'
	mov [sum], ax

	jmp print_sum




print_sum:
	mov eax, 4
	mov ebx, 1
	mov ecx, sum
	mov edx, 2
	int 0x80

	jmp exit



exit:
	mov eax, 1
	xor ebx, ebx
	int 0x80
