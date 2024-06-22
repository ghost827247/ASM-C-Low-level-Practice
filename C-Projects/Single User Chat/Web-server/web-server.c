#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Create Socket: socket()

// Set Reuse: setsockopt()

// Bind socket: bind()

// Listen for people: listen()

// accept()

// Recv()

// send()


const char* headers_404 = "HTTP/1.1 404 Not Found\r\n" // Headers For 404
							"Content-Type: text/html\r\n"
							"Connection: Close\r\n"
							"\r\n";


const char* headers_200 = "HTTP/1.1 200 OK\r\n" // Headers For 200
						  "Content-Type: text/html\r\n"
						  "\r\n\r\n";

const char* headers_200_css = "HTTP/1.1 200 OK\r\n" // Headers For 200
						      "Content-Type: text/css\r\n"
						      "\r\n\r\n";



void Handle_client(FILE* file, int client_fd) {
	char buffer[1024];
	// Send the 200 Headers

	// Read From Html File and send it
	while(fgets(buffer, sizeof(buffer), file) != NULL) {
		send(client_fd, buffer, strlen(buffer), 0);
	}
	fclose(file);
	close(client_fd);
}


void not_found(FILE* file, int client_fd) {
	char buf[1024];
	while(fgets(buf, sizeof(buf), file) != NULL) {
		send(client_fd, buf, strlen(buf), 0);
	}
	fclose(file);
	close(client_fd);
}


int main(int argc, char* argv[])
{
	int server_fd, client_fd;
	struct sockaddr_in Address, cleint_addr;
	FILE* file;

	int opt = 1;
	socklen_t addrlen = sizeof(Address);
	socklen_t clientAddr_len = sizeof(client_fd);
	

	int portno = 8080;

	// Create Socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Failed Creating Socket");
		return 1;
	}
	printf("Socket Created!\n");

	// Set The Socket Port/Address To Be reuseable
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		printf("Failed Setting Reuse");
		return 1;
	}

	// Setup Struct values
	Address.sin_family = AF_INET; // Use IPV4
	Address.sin_port = htons(portno); // Convert Port Number 
	Address.sin_addr.s_addr = INADDR_ANY; // Listen On all Interfaces

	// Bind the Socket to Port and address
	if((bind(server_fd, (struct sockaddr*)&Address, sizeof(Address))) < 0) {
		printf("Failed Bidning");
		return 1;
	}

	// Listen For Connections, upto 5 users
	if((listen(server_fd, 5)) < 0) {
		printf("Failed Listing");
		return 1;
	}
	printf("Listening On 0.0.0.0:%d\n\n", portno);
	printf("===========================\n\n");

	while(1) {
		// Accept Client
		if((client_fd = accept(server_fd, (struct sockaddr*)&cleint_addr, &clientAddr_len)) < 0) {
			printf("Failed Accepting");
			return 1;
		}

		printf("Connection From %s:%d\n", inet_ntoa(cleint_addr.sin_addr), ntohs(cleint_addr.sin_port));

		// Buffer To Hold Request
		char req_buffer[4096];

		// Recv Request From Client
		recv(client_fd, req_buffer, sizeof(req_buffer), 0);

		char *method;
		char* filename;

		// Get the Method
		method = strtok(req_buffer, " ");
		// Then Get the Filename They Want
		filename = strtok(NULL, " ");

		// Get The Length Of Filename
		int file_len = strlen(filename);

		// Check if File Name Begins with / and if it does
		if (filename[0] == '/') {
			// Overwrite the filename with the name minus the /
			memmove(filename, filename + 1, file_len);
			// Null Terminate it
			filename[file_len - 0] = '\0';
		}

		// Just Debug Shit
		printf("METHOD: %s\n", method);
		printf("RESOURCE: %s\n\n", filename);

		const char* headers_to_send = headers_404;

		if(strstr(filename, ".css") != NULL) {
			headers_to_send = headers_200_css;
			printf("css file");
		} 
		else if (strstr(filename, ".html") != NULL) {
			headers_to_send = headers_200;
		} 

		// Open The Request File
		file = fopen(filename, "r");

		// If It Exists Handle It
		if (file != NULL) {
			send(client_fd, headers_to_send, strlen(headers_200), 0); 
			Handle_client(file, client_fd);
		} else { // Else Send a 404
			send(client_fd, headers_to_send, strlen(headers_404), 0);
			FILE* file_404 = fopen("404.html", "r");
			not_found(file_404, client_fd);
		}

	}
	

	
	

	return 0;
}
