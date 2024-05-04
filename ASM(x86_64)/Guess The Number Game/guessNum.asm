section .data
;===================VARIABLES FOR TITLE/BOX========================
	title db "||  Guess The Number Between 1-10  ||", 10, 0
	title_len equ $ -title

	life_prmpt db "||        You Have 3 Lifes!        ||", 10, 0
	life_len equ $ -life_prmpt


	wall db "=====================================",10, 0
	wall_l equ $ -wall

	side_wall db "||                                 ||", 10, 0
	side_len equ $ -side_wall

;====================================================================

	input db "[*] Enter Number: ", 0
	input_l equ $ -input

	correct db "[*] Well Done! The Number was 4", 10, 0
	correct_l equ $ -correct

	incorrect db "[!] You Lose! Would you like To Play Again: ", 0
	incorrect_l equ $ -incorrect

	scanFormat db "%d", 0  ; Format For ScanF to Store Number as numeric

	num dd 9  ; Holds Correct Number (Will Add Rand Function Tomorrow)
	lifes dd 3 ; Holds Lifes Value

	high_prmpt db "[!] Higher", 10, 0
	low_prmpt db "[!] Lower", 10, 0

;=====================================================================
	new_line db "  ", 10, 0


section .bss
	user_num resd 1               ; Buffer To Hold Input Number (holds 4 bytes, 32 bits)
	choice resb 1                 ; Buffer To Hold Users Choice
	


section .text
	GLOBAL _start
	extern scanf
	extern printf

_start:
	jmp build_box                  ; Jump To Build Box Function



ret_area:                          ; Area To Retun To if Number != Correct Number
	mov edx, 0                     ; Move 0 into EBX to compare
	mov ebx, [lifes]               ; Move Lifes Left Into EDX
	cmp ebx, edx                   ; Compare Lifes Left Against 0
	je game_over                        ; Jump To Exit If lifes == 0
 
	push input                     ; Push Input String Onto Stack For PrintF to Grab
	call printf                    
	add esp, 4                     ; Add 4 bytes to Stack Pointer to put it back to where it was before push

	push user_num                  ; Push User_number memory address Onto Stack So ScanF knows where to save too
	push DWORD scanFormat          ; Push Format specifyer onto stack so scanF knows to save as integer
	call scanf
	add esp, 12                    ; Clean Up Stack

	mov ebx, [user_num]            ; Move User Inputted Number Into EBX
	mov edx, [num]                 ; Move hardCoded Number into EDX to compare

	cmp ebx, edx                   ; Compare User Number To Correct Number

	je correct_prmpt               ; if User_num == Correct Number Jump To Print "Well Done"
	jg lower                       ; If Lower Jump to print "lower"
	jl higher                      ; If Higher Jump To Print "Higher"



correct_prmpt:                     ; Area To Jump Too if Number Is Correct
	mov eax, 4
	mov ebx, 1
	mov ecx, wall                  ; Print Wall
	mov edx, wall_l
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, correct               ; Print Well Done Message
	mov edx, correct_l
	int 0x80

	mov eax, 4
	mov ebx, 1                     ; Print Wall
	mov ecx, wall
	mov edx, wall_l
	int 0x80



	jmp exit                       ; Jump To Exit



higher:                            ; Area To Jump If User_num higher then correct Number
	dec DWORD [lifes]              ; Decrement lifes by 1
	push high_prmpt                ; Push String Onto Stack For PrintF
	call printf
	add esp, 4                     ; Clean Up Stack

	push new_line
	call printf
	add esp, 4

	jmp ret_area                   ; Jump Back To Area to input another Number

lower:                             ; Area To Jump To if Number Lower
	dec DWORD [lifes]              ; lifes - 1
	push low_prmpt                 ; Push String Onto Stack
	call printf
	add esp, 4                     ; Clean Up Stack

	push new_line
	call printf
	add esp, 4
	
	jmp ret_area                   ; Jump Back To Input Area

game_over:                         ; Game Over Area
	mov eax, 4
	mov ebx, 1
	mov ecx, incorrect
	mov edx, incorrect_l
	int 0x80

	mov eax, 3
	mov ebx, 0
	mov ecx, choice
	mov edx, 2
	int 0x80

	mov al, [choice]
	sub al, '0'	

	mov esi, 3
	mov ecx, [lifes]

add_lifes:
	inc ecx
	dec esi
	jnz add_lifes

	mov [lifes], ecx
	cmp al, 1
	je _start
	jne exit
	


build_box:                         ; Function To Build Title Box
	mov eax, 4
	mov ebx, 1
	mov ecx, wall                  ; Print Top Wall
	mov edx, wall_l
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, title
	mov edx, title_len             ; Print Title
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, side_wall             ; Print Side Wall
	mov edx, side_len
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, life_prmpt            ; Print Amount Of Lifes String
	mov edx, life_len
	int 0x80

	mov eax, 4
	mov ebx, 1
	mov ecx, wall
	mov edx, wall_l                ; Print Bottom Wall
	int 0x80

	jmp ret_area


exit:
	mov eax, 1
	xor ebx, ebx
	int 0x80