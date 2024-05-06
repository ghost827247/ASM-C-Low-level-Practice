#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int open_save(FILE *file, int client_fd, const char* filename) { // Currently Cant Test Till i Make The Client
	file = fopen(filename, "w"); // Open FileName User Sent in write mode

	char file_buffer[BUFFER_SIZE]; // Buffer For Incoming File data
	ssize_t bytes_read; // Var For Amount OF bytes Read

	while(1) {
		bytes_read = recv(client_fd, file_buffer, sizeof(file_buffer), 0); // Receive Bytes From Client
		
		if (bytes_read <= 0) { // If Bytes_read <= 0 he is done sending
			break;
		}
		fprintf(file, "%s\n", file_buffer); // Write Data To File
	}
	fclose(file); // Close File
}
	

int open_send(FILE *file, int client_fd, const char* filename) {
	file = fopen(filename, "r"); // Open File In Read Mode
	if (file == NULL) {
		char error_msg[2048] = "File Doesnt Exist!"; // String To Send To Client If the file isnt in folder
		send(client_fd, error_msg, strlen(error_msg), 0); // Send "doesnt exist" To Client
		return 1; // Return 1, so we know it failed
	}

	char file_buffer[BUFFER_SIZE]; // Buffer For file data
	ssize_t bytes_read;
	while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) { // Read Data From File until it reaches 0
        send(client_fd, file_buffer, bytes_read, 0); // Send the data to Client
    }

    fclose(file); // Close File
    return 0; // Return 0 so we know it succeded
}

void error(const char* msg) { // Function To print Error Messages
	perror(msg);
	exit(EXIT_FAILURE);
}


int main(int argc, char const *argv[]) {
	int server_fd, client_fd, opt = 1;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t serv_len = sizeof(serv_addr);
	socklen_t client_len = sizeof(client_addr);
	int portno = atoi(argv[1]); // Convert Port From Ascii To int

	FILE *file; // Pointer to a FILE data Type

	char network_buffer[BUFFER_SIZE] = { 0 }; // Buffer For Data Coming From network
	char file_name_buffer[BUFFER_SIZE] = { 0 }; // Buffer To Hold the File Name

	char usage[2028] = "[*] Usage: get <filename>";
	
	

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Create a TCP socket using IPv4
		error("Failed Creating Socket");
	}

	if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) < 0) { // Set socket to be reuseable
		error("Failed Set Sock Opt");
	}

	serv_addr.sin_family = AF_INET; // Set Internet Domain To IPv4
	serv_addr.sin_port = htons(portno); // Convert Port Number to network Byte Order
	serv_addr.sin_addr.s_addr = INADDR_ANY; // Declare We Want the socket to use any Hosts IP/Interface

	if ((bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) { // Bind IP/port  To Socket
		error("Failed Binding Socket");
	}

	listen(server_fd, 0); // Listen For Connection With Back Log Of 0
 
	if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) { // Accept User Connection and Save File Descriptor Number Into Client_fd
		error("Failed Accepting Client!");
	}

	printf("[*] Connection From: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); // Print Client IP and Port
	printf("[*] Waiting For Client Commands\n");
	send(client_fd, usage, sizeof(usage), 0);

	while(1) {
		bzero(network_buffer, sizeof(network_buffer)); // Clear Data Coming From Network Buffer
		ssize_t bytes_read = recv(client_fd, network_buffer, sizeof(network_buffer), 0); // Recv Data From Client
		
		network_buffer[bytes_read] = '\0'; // Null Terminate User Command

		if (strncmp(network_buffer, "get ", 4) == 0) { // Check If User Sent "get"
			strcpy(file_name_buffer, network_buffer + 4); // Copy Filename From User Into differnt Buffer, buffer + 4 = anything after "get "
			file_name_buffer[strcspn(file_name_buffer, "\n")] = '\0'; // Null Terminate Filename
			printf("[*] User Wants: %s\n", file_name_buffer); // Print What File User Wants
			if ((open_send(file, client_fd, file_name_buffer)) == 0) { // Jump To Open and Send Function
				printf("[*] File Sent Succesfully\n"); // If the Function Returns 0 it succeded
			} 

		}
		else if (strncmp(network_buffer, "put ", 4) == 0) { // Check If User Entered "put"
			strcpy(file_name_buffer, network_buffer + 4); // Copy File Name
			file_name_buffer[strcspn(file_name_buffer, "\n")] = '\0'; // Null terminate
			printf("[*] User Wants: %s\n", file_name_buffer);
			if ((open_save(file, client_fd, file_name_buffer)) == 0) { // Jump To Open And Save Function
				printf("File Saved Succesfully");
			}

		}

		break; // Break Once Done Client has been sent the data or sent the data
	}
	close(server_fd); // Close Server Stream
	close(client_fd); // Close Client Stream


	return 0;
}