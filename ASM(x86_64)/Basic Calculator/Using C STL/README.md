This Program Does the same as the Pure ASM calculator, Except it Uses PrintF and ScanF from the C stl, 

This Makes It Much Easier And takes away Having To Convert Numbers Ourself ect

Dont Mind the Barrage Of Comments, just added heaps as im trying to get used to commenting(i used to be very lazy and would forget what my program does after a while)

compile with

    nasm -f elf32 calc.asm
    ld -m elf_i386 -o calc calc.o -lc -dynamic-linker /lib/ld-linux.so.2
    ./calc

  This Program Also Has Same Basic Error Handling, Does Need More Though
