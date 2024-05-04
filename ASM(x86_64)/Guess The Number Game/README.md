Basic Guess The Number Game in x86_32 Assmebly For Linux, It Currently Doesnt Have a Random Number Generator but i plan to add it
Like Usual You Get 3 lifes To figure out the Number between 1-10, It Does Use Functions From The C STL So need to link with it

    nasm -f elf32 guessnum.asm
    ld -m elf_i386 -o GuessNum guessNum.o -lc -dynamic-linker /lib/ld.linux.so.2
    ./GuessNum

I Plan On Adding New Features If i Get Time As Im Getting Back Into C after a bit of time off
