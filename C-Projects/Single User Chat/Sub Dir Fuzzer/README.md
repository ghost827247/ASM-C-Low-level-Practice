This Is My First Go at a Web Fuzzer, It Currently Can Only Fuzz For Sub Domains and Sub Directorys But i Plan to Add Support For fuzzing for files ect


It Has Basic Threading Which Needs Some Fixing as it can sometimes Hog All The Computers Resources

If The Site Repsondes with a 200 Or 302 It Prints the directory it found


COMPILING
It Uses the LIBCURL And PTHREAD library So Make Sure to link with it
  
    sudo apt-get install libcurl4-openssl-dev
    gcc -o fuzzer fuzzer.c -lcurl -pthread
USAGE

    ./fuzzer http://example.com wordlist.txt <Amount Of Threads> <Scan Choice> (subdir, Subdomain)

![Screenshot_2024-05-13_00-02-03](https://github.com/zevuxo1/ASM-C-Low-level-Practice/assets/155918223/e5252c14-589e-41d7-a907-0086b501dd6e)

Also Dont Mind All The Ugly ANSI codes i used in the string instead of using the defined values, ill change that
The logo and UI need updating too its kind of ugly
