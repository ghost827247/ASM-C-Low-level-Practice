#include <stdio.h> // For Basic I/O functions
#include <stdlib.h> // Used For data types and syscalls
#include <string.h> // Used For Some Useful functions like sizeof(), strlen()
#include <netinet/in.h> // contains constants and structures needed for internet domain addresses
#include <sys/socket.h> // Includes a Number of structs for socket programming
#include <unistd.h>
#include <arpa/inet.h> // Needed For converting network Byte IP  to normal Bytes




int main(int argc, const char *argv[]) {
	int server_fd, client_fd; // Variables To Hold the Client and server File Descriptors. the values returned From socket(), accept()
	struct sockaddr_in address, client_addr; // Structure Defined in sys/socket.h, containing Internet Domain settings Defnition below 

	/** struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
	};
	**/



	char buffer[300] = { 0 }; // Buffer To Hold  input from over the socket for reading, { 0 } = set all bytes to 0
	char resbuffer[300] = { 0 }; // Buffer To Hold User Input to Send Back over socket.
	ssize_t valread; // Dont Need This Anymore

	int opt = 1; // Value For setsockopt
	socklen_t addrlen = sizeof(address);
	socklen_t client_Addrlen = sizeof(client_addr);
	int portno = atoi(argv[1]); // Convert user port Number From Ascii to INT

	if (argc < 2) {
		fprintf(stderr, "Usage: ./server <portno>");  // If argv != 2(if there is no port number after ./server) exit the program
		return 1;
	}


	// Create a Socket And Check if it was succesfull
	// int socket(int domain, int type, int protocol);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Create a Socket Of type IPv4(AF_INET), using TCP(SOCK_STREAM : UDP = SOCK_DGRAM), FD gets saved into server_fd
		perror("Failed Creating Socket");
		exit(EXIT_FAILURE);
	}

	// Create REUSEADDR
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { // This Helps Prevent Errors Such as "Port Is already In User"
		perror("Failed Attacking Socket");
		exit(EXIT_FAILURE);
	}

	// Define Values In Struct
	address.sin_family = AF_INET; // The variable serv_addr is a structure of type struct sockaddr_in. This structure has four fields. 
								  // The first field is short sin_family, which contains a code for the address family. It should always be set to the symbolic constant AF_INET.


	address.sin_port = htons(portno); // Second Value of sockaddr_in, is a unsigned short sin_port, it must be converted to a network byte order by using htons
	address.sin_addr.s_addr = INADDR_ANY; // This Field Contains the IP of the Host who is running the server, 
				// INADDR_ANY means we can accept connections from any network interface


	// Bind Ip/Port
	if ((bind(server_fd, (struct sockaddr*)&address, sizeof(address))) < 0) { // Forcefully attach the socket to the ip/port, it gets the IP and PORT from the sockaddr_in struct
		perror("Failed Binding Socket");
		exit(EXIT_FAILURE);
	}

	// Start Listner

	if ((listen(server_fd, 5)) < 0) { // Start Listerning For Connections on the socket, 5 = backlog of 5
		perror("Listen Fail");
		exit(EXIT_FAILURE);
	}

	printf("Listening on 127.0.0.1:%d\n", portno);



	// Accept Client
	if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_Addrlen)) < 0) { // Blocks Program Flow Until a connection occurs, once a connection occurs the Handle to the client stream is saved in client_fd
		perror("Failed To Accept Client"); // All Next sends/reads should come from this FD
		exit(EXIT_FAILURE);
	}

	printf("Got a Connection From %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


	while(1) {
		bzero(buffer, sizeof(buffer));  // Set Entire Buffer Back to 0, each loop 
		recv(client_fd, buffer, sizeof(buffer), 0); // Receive Messages From Client and store in buffer
		printf("From Client: %s\n", buffer); // Print Message

		printf("Enter Reply: "); 
		fgets(resbuffer, sizeof(resbuffer), stdin); // Read From STDIN and Store In resbuffer For sending over network
		send(client_fd, buffer, strlen(buffer), 0); // Send Message To Client, See How we use client_fd to specify which connection to send too
		bzero(resbuffer, sizeof(resbuffer)); // Clear Reply buffer
	}

	close(server_fd); // Close Server Stream
	close(client_fd); // Close Client Stream

	return 0;
}



