This is a Basic Calculator Made With Pure x86 ASM for linux, it only supports Single Digit Input and Output As im Still Learning How To Convert Multi Digit Numbers To Compile this Use

    nasm -f elf32 calc.asm
    ld -m elf_i386 -o calc calc.o
    ./calc

I Plan On Adding Multi Digit Support As Time Goes On And To Add Better Error Handling As it Has 0 Right Now
