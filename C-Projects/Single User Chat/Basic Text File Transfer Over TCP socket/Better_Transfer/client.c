#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_SIZE 2049

// int get_save()
int get_save(FILE *file, const char* filename, int client_fd) { // Function To Save The File We Want
	
	ssize_t bytes_read; // Var To Hold Amount OF Bytes Read From Socket
	char network_buffer[MAX_SIZE]; // Buffer To Put Data From Server Into
	memset(network_buffer, 0, sizeof(network_buffer)); // Set network_buffer to 0 (Causes Problems For Compare statment if not cleared)

	bytes_read = recv(client_fd, network_buffer, sizeof(network_buffer), 0); // Read Message From File, incase its the "doesnt exist message"
    if (bytes_read <= 0) { 
        // Error or connection closed by client
        return -1;
    }

    if (strcmp(network_buffer, "File Doesnt Exist!") == 0) { // Compare message From Server To Check If File Existed
		printf("[!] File Doesnt Exist"); // If == 0 It Means True
		return 1;
	}

	file = fopen("file.txt", "w"); // Open File In Write Mode
	if (file == NULL) {
		perror("Failed Opening File");
		
	}

	fprintf(file, "%s\n", network_buffer); // Write To File InCase It was a correct File


	while(1) {
		bytes_read = recv(client_fd, network_buffer, sizeof(network_buffer), 0);

		if (bytes_read <= 0) {
			break;
		}

		fprintf(file, "%s\n", network_buffer);
	}
	fclose(file);
	return 0;
}

// int put_send()
int put_send(FILE* file, const char* filename, int client_fd) {
	ssize_t bytes_read; // Var To Hold To Keep Count Of amount of bytes read
	char network_buffer[MAX_SIZE]; // Buffer To Hold incoming data
	char file_buffer[MAX_SIZE]; // Buffer To Hold File Contents To Send To User
	file = fopen("client.txt", "r");
	printf("%s", filename);

	if (file == NULL) {
		printf("[!] File Doesnt Exist: Check If File Is In Current Directory");
		return 1;
	}

	memset(network_buffer, 0, sizeof(network_buffer));
	memset(file_buffer, 0, sizeof(file_buffer));

	// bytes_read = recv(client_fd, network_buffer, sizeof(network_buffer), 0);

	// if (bytes_read <= 0) {
	// 	return 1;
	// }

	rewind(file);
	while (fgets(file_buffer, sizeof(file_buffer), file) != NULL) {
		send(client_fd, file_buffer, strlen(file_buffer), 0);
	}

	fclose(file);
	return 0;
}

void error(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
	int client_fd, status;
	struct sockaddr_in address;
	int portno = atoi(argv[1]);
	char command[200] = { 0 };
	char filename[50] = { 0 };
	char usage[2048];
	FILE *file;

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error("Failed Creating Socket");
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(portno);

	if ((inet_pton(AF_INET, "192.168.1.30", &address.sin_addr)) <= 0) {
		error("Failed Conveting IP");
	}

	if ((status = connect(client_fd, (struct sockaddr*)&address, sizeof(address))) < 0) {
		error("Failed Connecting To Server!");
	}

	ssize_t bytes_recv = recv(client_fd, usage, sizeof(usage), 0);
	usage[bytes_recv] = '\0';
	printf("%s\n", usage);
	printf("=============================\n");
	

	printf("$> ");
	fgets(command, sizeof(command), stdin);
	if (strncmp(command, "get ", 4) == 0) {
		printf("You Want Get\n");
		strcpy(filename, command + 4);
		filename[strcspn(command, "\n")] = '\0'; // Null Terminate Filename
		printf("%s", filename);
		send(client_fd, command, strlen(command), 0);
		if ((get_save(file, filename, client_fd)) == 0) {
			printf("[*] Succesfully saved File");
		}
	}

	else if (strncmp(command, "put ", 4) == 0) {
		printf("You Want Put\n");
		strcpy(filename, command + 4);
		filename[strcspn(command, "\n")] == '\0';
		send(client_fd, command, strlen(command), 0);
		put_send(file, filename, client_fd);
	}
	
	close(client_fd);

	return 0;
}
