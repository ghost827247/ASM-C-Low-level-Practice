#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m" // ANSI codes For Colored Output
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD    "\033[1m"

int count_words(FILE *file, char *result) { // Function To Count the amount of lines in the text file
	int i = 1;
	char line[60];


	while ((result = fgets(line, sizeof(line), file)) != NULL) {
		i++; // While Line != EMPTY we update the line count
	}
	rewind(file); // After Its Down We Rewind the File back to beginning for fuzzing

	return i; // Return line count
}


char* get_time() { // Function To Get the Current Time 
	char* time_string = (char *)malloc(sizeof(char) * 9);
	if (time_string == NULL) {
		printf("Failed Allocating Memory!");
	}

	time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	snprintf(time_string, 9, "%02d:%02d:%02d", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	return time_string;
}

void scan_site(FILE *file, char *baseurl) { // Function To Fuzz Site For Sub Dirs
	char line[60]; // Buffer To Hold line From File
	char *result; // Used to Check if line From file == NULL

	CURL *curl; // Create a pointer to a CURL Handle (Just An Easy-to-use Strucutre for handling web requests)
	CURLcode res_code; // Holds Response Code From request
	curl = curl_easy_init(); // Set Up Curl Object

	char *time; // Var To Hold Time thats returned From get_time()
	time = get_time();

	printf("[!] Starting Scan At %s\n", time); // print Starting time
	printf("\033[1m\x1b[36m=============================================================\x1b[0m\n");
	printf("||  [*] DIRECTORY %-20s\t[*] RESPONSE CODE  ||\n", " ");                                  // Just Some UI Stuff
	printf("\033[1m\x1b[36m=============================================================\x1b[0m\n");
	

	// If Curl Not Empty We Are Set To Go
	if(curl) {
		while ((result = fgets(line, sizeof(line), file)) != NULL) { // Read Line From File Until NULL (End Of File)
			line[strcspn(line, "\n")] = 0; // Remove new Line char from line
			char url[200]; // Buffer For URL
			snprintf(url, sizeof(url), "%s/%s/", baseurl, line); // Append line From File To the URl From argv[1]

			curl_easy_setopt(curl, CURLOPT_URL, url); // Set Up the URL we want to use in the request
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Let CURL Know We want to follow redirects
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Use HEAD request (We Dont Want the HTML contents)

			res_code = curl_easy_perform(curl); // Execute the request

			if (res_code == CURLE_OK) { // Check if request was succesfull or a fail
				long res_code; // Response Code Is Stored as a long int
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code); // Get the response Code and store it in res_code

				if (res_code == 200 || res_code == 302) { // Check if Response Was a 200 or 302

					printf("[\033[1m\x1b[32m*\x1b[0m] Found: /%-20s\tResponse Code: \033[1m\x1b[32m%ld\x1b[0m\n", line, res_code); // Print Sub Dir we found and the code
				} 
				
			}
		}
	}
	printf("\033[1m\x1b[36m=============================================================\x1b[0m\n");
	time = get_time(); // Get Time Again
	printf("[!] Scan Finished At %s\n", time); // Print End Time
	free(time);
	curl_easy_cleanup(curl); // Invoke CURL to cleanup 

}

int main(int argc, char *argv[])
{
	logo(); // Print Logo (needs redoing cause ugly)

	char *result; 
	FILE *file; // Pointer too a FILE type
	char *baseurl = argv[1]; // Save The URL user entered into Base_url
	char *filename = argv[2]; // Save The file to use as wordlist
	int wc;

	if (argc < 2) {
		printf("\033[1m\x1b[31m[!]\x1b[0m USAGE: %s <url> <wordlist>\n", argv[0]); // if user didnt arguments exit
		printf("\033[1m\x1b[31m[!]\x1b[0m %s http://example.com words.txt", argv[0]);
		return 1;
	}
	
	file = fopen(filename, "r"); // Open Wordlist In Read Mode
	if (file == NULL) {
		printf("\x1b[31m[!]\x1b[0m Failed Opening File!");
		return 1;
	}
	wc = count_words(file, result); // Count Amount of lines in file

	printf("\n");

	printf("\033[1m\x1b[36m=============\x1b[0m\x1b[31mTARGET INFO\x1b[0m\033[1m\x1b[36m=============\x1b[0m\n"); // Print Target infomation
	printf("[+] URL: %s\n", baseurl);
	printf("[+] METHOD: Get\n");
	printf("[+] WORDLIST: %s\n", filename);
	printf("[+] WORDCOUNT: %d\n", wc);
	printf("\033[1m\x1b[36m========================\x1b[0m\033[1m\x1b[36m=============\x1b[0m\n");

	printf("\n\n");

	scan_site(file, baseurl); // Start Fuzzing
	

	
	fclose(file); // Close File

	
	return 0;
}

void logo() {
	printf("||  _________    ___.   ___________                                    ||\n");
    printf("|| /   _____/__ _\\ |__ \\_   _____/_ __________________ ___________     ||\n");
    printf("|| \\_____  \\|  |  \\ __ \\ |    __)|  |  \\___   /\\___   // __ \\_  __ \\   ||\n");
    printf("||  /        \\  |  / \\_\\ \\|     \\ |  |  //    /  /    /\\  ___/|  | \\/  ||\n");
    printf("|| /_______  /____/|___  /\\___  / |____//_____ \\/_____ \\\\___  >__|     ||\n");
    printf("||        \\/          \\/     \\/              \\/      \\/    \\/          ||\n");
    printf("=========================================================================\n");

}