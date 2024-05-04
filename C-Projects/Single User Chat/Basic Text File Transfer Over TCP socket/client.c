#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
	int client_fd, status;;
	struct sockaddr_in address;
	FILE *file;
	char buffer[300] = { 0 };
	char name[50];


	if (argc >= 3) {
		strcpy(name, argv[2]);
		
	} else {
		printf("USAGE: ./client <port> <file>");
		return 1;
	}
	int portno = atoi(argv[1]);
	
	


	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		error("[!] Failed Creating Socket");
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(portno);

	if ((inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)) < 0) {
		perror("[!] Failed Converting IP");
	}

	if ((status = connect(client_fd, (struct sockaddr*)&address, sizeof(address))) < 0) {
		perror("[!]Failed Connecting\n[!]Make Sure Server Is Running!\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Connected To Server, File Transfer Starting Now!\n");

	if ((file = fopen(name, "r")) == NULL) {
		error("Failed Opening File!\nMake Sure File Is in Same Folder!\n");


	}

	while(fgets(buffer, 300, file) != NULL) {
		if ((send(client_fd, buffer, sizeof(buffer), 0)) < 0){
			error("Failed Sending Data");
		}
	}


	printf("[*] Succesfully Sent %s To Server!", name);

	return 0;
}