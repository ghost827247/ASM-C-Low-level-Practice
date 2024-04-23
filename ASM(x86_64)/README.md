Basic Assembly Code Will Be Stored Here


CALC.ASM
  To Create An executable for This code
    nasm -f elf32 calc.asm

    ld -m elf_i386 -o calc.o calc

    ./calc

  This Project Can Only handle single digit input and output as i am still learning about how to convert digits >9 to ascii for printing using Pure ASM
