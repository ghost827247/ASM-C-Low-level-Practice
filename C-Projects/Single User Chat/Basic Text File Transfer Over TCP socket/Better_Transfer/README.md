This Is a Better File Transfer that kind of acts like a FTP server, it has 2 basic commands, GET and PUT

Right Now It Only Does 1 Loop before exiting on both sides but that is to change, i plan to add another command exit which closes the server
Also On the client side for putting a file i cant get the filename to send to properly pass to the function for some reason, it just doesnt see it as a valid name
even tho when i print it, it is the correct name?

so if you want to send a certain file if anyone even sees this change
        
       LINE 57: file = fopen("client.txt", "r"); 
       inside the put_send function in client.c


    gcc -o server server.o
    gcc -o client client.c

    ./server <port To Listen On>
    ./client <port server is on>

    USAGE FOR CLIENT
          get <filename>
          put <filename> (make sure file is the same Directory As client Binary)


