#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int status, client_fd;
	struct sockaddr_in serv_address;
	int portno = atoi(argv[1]);

	char buffer[300] = { 0 };
	char resbuffer[300] = { 0 };

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failed Creating Socket");
		exit(EXIT_FAILURE);
	}

	serv_address.sin_port = htons(portno);
	serv_address.sin_family = AF_INET;

	if ((inet_pton(AF_INET, "127.0.0.1", &serv_address.sin_addr)) <= 0) {
		perror("Failed Converting IP");
		exit(EXIT_FAILURE);
	}

	if ((status = connect(client_fd, (struct sockaddr*)&serv_address, sizeof(serv_address))) < 0) {
		perror("Failed Connecting");
		exit(EXIT_FAILURE);
	}

	while(1) {
		bzero(buffer, sizeof(buffer));
		printf("Enter Message: ");
		fgets(buffer, sizeof(buffer), stdin);
		send(client_fd, buffer, strlen(buffer), 0);

		recv(client_fd, resbuffer, sizeof(resbuffer), 0);
		printf("From Server: %s\n", resbuffer);
		bzero(resbuffer, sizeof(resbuffer));
	}

	


	return 0;
}



