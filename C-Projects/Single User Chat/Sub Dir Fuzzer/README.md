This Is My First Go at a Web Fuzzer, It Currently Can Only Fuzz For Sub Domains and Sub Directorys But i Plan to Add Support For fuzzing for files ect


It Has Basic Threading Which Needs Some Fixing as it can sometimes Hog All The Computers Resources

If The Site Repsondes with a 200 Or 302 It Prints the directory it found


COMPILING
It Uses the LIBCURL And PTHREAD library So Make Sure to link with it
  
    sudo apt-get install libcurl4-openssl-dev
    gcc -o fuzzer fuzzer.c -lcurl -pthread
USAGE

    ./fuzzer http://example.com wordlist.txt <Amount Of Threads> <Scan Choice> (subdir, Subdomain)

![Screenshot_2024-05-17_01-02-47](https://github.com/zevuxo1/ASM-C-Low-level-Practice/assets/155918223/fd0982f4-bd5a-477a-89e3-58d19d9668b4)


