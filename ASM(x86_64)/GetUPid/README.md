As Im Still Learning About the Stack and pushing, popping and cleaning it Up This tiny project helped me grasp the absolute Basics,

All it Does it Use SYS_GETPID and SYS_GETUID System Calls to get the current process ID and the Current User ID 

It Also Also External C Standard Library For PrintF as im still learning how to convert multi Digit Numbers In Pure ASM

To Compile This Use

    nasm -f elf32 GetUPid.asm
    ld -m elf_i386 -o getUPid GetUPid.o -lc -dynamic-linker /lib/ld-linux.so.2
    ./GetUPid
