section .data
	pid_prompt db "Process ID: %d", 10, 0
	uid_prompt db "User ID: %d", 10, 0

section .text
	GLOBAL _start
	extern printf

_start:
	mov eax, 20
	int 0x80

	mov ebx, eax

	push ebx
	push dword pid_prompt
	call printf
	add esp, 8

	mov eax, 24
	int 0x80
	mov ebx, eax

	push ebx
	push uid_prompt
	call printf

	mov eax, 1
	xor ebx, ebx
	int 0x80