This is Just a Basic File Transfer Program, It is my 2nd project to learn socket programmig
It has a Client and server C file To compile, 
It Currently Only Can handle Text Files but im going to add a fix to send any type of file over the socket

    USAGE
      gcc -o server server.c
      gcc -o client client.c

      ./server <Port To Listen On>
      ./client <Port Server Is Listening On> <file to send> // Make Sure File Is in Same Folder As Client program
I Will Also Be Adding Support To make the file be saved the same name as the file being sent from client.
But you do get a prompt to change the name after it gets sent

