#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <netinet/in.h>
#include <sys/socket.h>


void error(const char* msg) { // Function For Printing Error Messages When Something Goes Wrong
	perror(msg);
	exit(EXIT_FAILURE);
}


// Argc = Argument count, Argv = Actual Arguments Passed
int main(int agrc, const char* argv[]) {
	int server_fd, client_fd, opt = 1; // Vars To hold server/client File Descrtiptor

	struct sockaddr_in address; // Declare Struct For Internet domain settings
	socklen_t addr_len = sizeof(address); // Get Size Of Address Struct
	int portno = atoi(argv[1]); // Convert Port Number to Int From Ascii
	char buffer[300] = { 0 }; // Buffer To Store File Contents into
	char choice; // Choice For Changing File Name
	char name[50]; // Buffer For Filename

	FILE *file; // Pointer to a FILE data type
	file = fopen("file.txt", "wb"); // Open File in Write Mode

	if(file == NULL) { // If File Doesnt return Anything it failed To Open
		error("Failed Opening File!");
	}

	// Create Socket
	// int socket(int domain, int type, int protocol);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Create Socket that uses IPv4-TCP
		error("Failed To Create Socket");

	}


	// Set Sockopt
	// setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) < 0) {
		error("Failed Setting REUADDR"); // Set Socket To ReuseAddress, stops errors like "Port In use"
	}


	// Set Struct variables
	address.sin_family = AF_INET; // indicate IP is a IPv4
	address.sin_port = htons(portno); // Set Port Number For Socket, htons = convert to network byte
	address.sin_addr.s_addr = INADDR_ANY; // Indicate We want to use any of the hosts IP's/interfaces, 127.0.0.1, 192.168.1.30 ect

	// Bind Socket To inteface
	//  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	if ((bind(server_fd, (struct sockaddr*)&address, sizeof(address))) < 0) { // Bind The Ip and Port To a socket
		error("Failed Binding Socket To interface");
	}


	// Listen For Connection
	// int listen(int sockfd, int backlog);
	listen(server_fd, 0); // Listen For Any Connections
	printf("Listening on: 127.0.0.1:%d", portno); 

	
	// int accept(int sockfd, struct sockaddr *_Nullable restrict addr, socklen_t *_Nullable restrict addrlen);
	if ((client_fd = accept(server_fd, (struct sockaddr*)&address, &addr_len)) < 0) { // Accept Clients Connection and save his FD into client_fd
		error("Failed Accepting Client!");
	}

	printf("Got a Connection, File Transfer Starting Now!\n");


	// Read Data From Client till NULL and Write to file
	//ssize_t recv(int sockfd, void buf[.len], size_t len, int flags);

	while(1) {
		int bytes = recv(client_fd, buffer, sizeof(buffer), 0); // Read All Bytes coming From Client

		if (bytes <= 0) { // If bytes equals 0, client is done sending
			break;
		}
		fprintf(file, "%s\n", buffer); // Write Client File Contents Into the Files Stream
		bzero(buffer, sizeof(buffer)); // Clear Buffer
	}

	printf("File Succesfully Transfered!\n");
	printf("Would You Like To Change The Name (y/n): ");
	scanf("%c", &choice); // Read User Choice

	if (choice == 'y') {
		char command[300]; // Buffer To Hold Command
		printf("Enter File Name To Change Too: "); 
		scanf("%s", name);
		sprintf(command, "mv file.txt %s", name); // Concat File Name into command string
		if (system(command) == 0) { 
			printf("Succesfully Changed File Name too: %s\n", name); //  Execute File mv command and print Success
		}
		else {
			error("Failed Changing Name");
		}
	}

	// Close Sockets

	close(server_fd); // Close Socket
	close(client_fd); // Close Client Connection


}


