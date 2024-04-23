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
	num1 resb 2
	num2 resb 2
	sum resb 2

	op resb 2

section .text
	GLOBAL _start

_start:
	mov eax, 4
	mov ebx, 1
	mov ecx, first
	mov edx, f_len
	int 0x80

	mov eax, 3
	mov ebx, 0
	mov ecx, num1
	mov edx, 2
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, second
	mov edx, s_len
	int 0x80

	mov eax, 3
	mov ebx, 0
	mov ecx, num2
	mov edx, 2
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, operator
	mov edx, op_len
	int 0x80

	mov eax, 3
	mov ebx, 0
	mov ecx, op
	mov edx, 2
	int 0x80

	mov bh, [op]
	sub bh, '0'

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
