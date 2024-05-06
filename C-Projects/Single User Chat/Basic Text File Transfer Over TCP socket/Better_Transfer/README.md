This Is a Better File Transfer that kind of acts like a FTP server, it has 2 basic commands, GET and PUT

The Server Code Is Finished But the Client Still Needs Work mainly on the save and send functions, im having problems with the "File Doesnt Exist!" Compare
Some Time It Works Sometimes it Doesnt


    gcc -o server server.o
    gcc -o client client.c

    ./server <port To Listen On>
    ./client <port server is on>

    USAGE
          get <filename>
          put <filename> (make sure file is the same Directory As client Binary)

if anyone sees this please help lol, this code is pissing me off
