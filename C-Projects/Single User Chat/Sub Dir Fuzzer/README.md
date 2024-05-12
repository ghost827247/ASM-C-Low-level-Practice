This Is My First Go at a Sub Directory Fuzzer, its goal is to use a wordlist of common sub directorys on a website and check for its excistence

Which Can lead To Finding Hidden Or Important files

I Plan To Add Threading To It To much it a bit quicker but for know it just reads a line from a text file, appends it to the url and sends the GET requests

If The Site Repsondes with a 200 Or 302 It Prints the directory it found

It Has 0 SUpport For HTTPS sites rn because its more just a learning project

COMPILING
It Uses the LIBCURL library So Make Sure to link with it
  
    sudo apt-get install libcurl4-openssl-dev
    gcc -o fuzzer fuzzer.c -lcurl
USAGE

    ./fuzzer http://example.com wordlist.txt

![Screenshot_2024-05-13_00-02-03](https://github.com/zevuxo1/ASM-C-Low-level-Practice/assets/155918223/e5252c14-589e-41d7-a907-0086b501dd6e)

Also Dont Mind All The Ugly ANSI codes i used in the string instead of using the defined values, ill change that
The logo and UI need updating too its kind of ugly
