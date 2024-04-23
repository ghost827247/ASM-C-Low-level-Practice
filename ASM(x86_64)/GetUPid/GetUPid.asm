section .data
	pid_prompt db "Process ID: %d", 10, 0
	uid_prompt db "User ID: %d", 10, 0
	; String Format For Process and User ID

section .text
	GLOBAL _start
	; External Function For C's PrintF
	extern printf

_start:
	; Make System Call To Get Process ID, 20 = SYS_GETPID
	mov eax, 20
	int 0x80

	; The PID is Stored Inside EAX after the call, so we move it to EBX for printing
	mov ebx, eax

	; Push PID Onto The Stack Then Push the String Onto the Stack For Printf to grab
	push ebx
	push dword pid_prompt
	call printf ; Call PrintF

	; Clean Up The Stack By Adding 8 Bytes To ESP (Stack pointer) putting It Back To Where it was before the push's
	add esp, 8

	 Make System Call To Get User ID, 24 = SYS_GETUID
	mov eax, 24
	int 0x80

	; Value Gets saved into eax, so we move it for printing
	mov ebx, eax

	; Push Items Onto The Stack for printF to grab
	push ebx
	push uid_prompt
	call printf

	; Exit Program
	mov eax, 1
	xor ebx, ebx
	int 0x80
