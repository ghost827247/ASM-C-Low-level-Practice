#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m" // ANSI codes For Colored Output
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD    "\033[1m"

struct ThreadArgs { // Struct For values to pass to function For Threading
    FILE* file;
    char* baseurl;
};

int count_words(FILE *file, char *result) { // Function To Count the amount of lines in the text file
	int i = 0;
	char line[60];
	while ((result = fgets(line, sizeof(line), file)) != NULL) {
		i++; // While Line != EMPTY we update the line count
	}
	rewind(file); // After Its Done We Rewind the File back to beginning for fuzzing
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


int check_if_up(char *url) {  // Quick Check To See If We Can Actually Reach The Website, If We Cant Just Exit program
	CURL* curl;
	CURLcode res_code;
	curl = curl_easy_init();
	
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		res_code = curl_easy_perform(curl);

		if(res_code == CURLE_OK) {
			long response_code;
			res_code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

			return (response_code == 200 || response_code == 302) ? 1 : 0; // One Liner If Else, if true return 1 else return 0
		} 

	}
	curl_easy_cleanup(curl);
	return 0;
}


void print_target_info(char* baseurl, char* type, char* filename, int wc, int thread_count) {
	printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "====================" ANSI_COLOR_RED ANSI_COLOR_BOLD"TARGET INFO" ANSI_COLOR_RESET ANSI_COLOR_BLUE ANSI_COLOR_BOLD"====================\n" ANSI_COLOR_RESET); // Print Target infomation
	printf("[+]" ANSI_COLOR_BOLD" URL: " ANSI_COLOR_RESET "%s\n", baseurl);
	printf("[+]" ANSI_COLOR_BOLD " METHOD: " ANSI_COLOR_RESET "HEAD\n");
	printf("[+]" ANSI_COLOR_BOLD" SCAN-TYPE: " ANSI_COLOR_RESET "%s\n", type);
	printf("[+]" ANSI_COLOR_BOLD" WORDLIST: " ANSI_COLOR_RESET "%s\n", filename);
	printf("[+]" ANSI_COLOR_BOLD " WORD-COUNT: " ANSI_COLOR_RESET "%d\n", wc);
	printf("[+]" ANSI_COLOR_BOLD " THREAD-COUNT: " ANSI_COLOR_RESET "%d\n", thread_count);
	printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "===================================================\n" ANSI_COLOR_RESET);
}


void* subdomain_Scan(void* arg) {
	struct ThreadArgs* args = (struct ThreadArgs*)arg;
	char line[60];
	char *result;
	//char *baseurl = baseurl;
	CURL *curl;
	CURLcode res_code;
	curl = curl_easy_init();
	FILE* file = args->file;
	char* baseurl = args->baseurl;

	if(curl) {
		while((result = fgets(line, sizeof(line), file)) != NULL) {
			line[strcspn(line, "\n")] = 0;
			char url[200];
			snprintf(url, sizeof(url), "http://%s.%s", line, baseurl);
			//printf("Reaching Out Too %s\n", url);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

			
			res_code = curl_easy_perform(curl);

			if (res_code == CURLE_OK) {
				// printf("CURLE OKE"); WORKS
				long response_code;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

				if (response_code == 200) {
					printf("[*] Found: %s  %-20s\tResponse Code: [%d\n]", line, " ", response_code);
				}
			}
		}
	}
	curl_easy_cleanup(curl);
}

void* subdir_scan(void* arg) { // Function To Fuzz Site For Sub Dirs
	struct ThreadArgs* args = (struct ThreadArgs*)arg;
	char line[60]; // Buffer To Hold line From File
	char *result; // Used to Check if line From file == NULL
	FILE* file = args->file;
	char* baseurl = args->baseurl;

	CURL *curl; // Create a pointer to a CURL Handle (Just An Easy-to-use Strucutre for handling web requests)
	CURLcode res_code; // Holds Response Code From request
	curl = curl_easy_init(); // Set Up Curl Object


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

				if (res_code == 200 || res_code == 302 || res_code == 301 || res_code == 201) { // Check if Response Was a 200 or 302

					printf("[" ANSI_COLOR_GREEN ANSI_COLOR_BOLD "*" ANSI_COLOR_RESET "] Found: /%-20s\tResponse Code: " ANSI_COLOR_GREEN ANSI_COLOR_BOLD "[%ld]\n" ANSI_COLOR_RESET, line, res_code); // Print Sub Dir we found and the code
				} 
				else if (res_code == 403 || res_code == 402) {
					printf("[" ANSI_COLOR_RED  ANSI_COLOR_BOLD "*" ANSI_COLOR_RESET "] Found: /%-20s\tResponse Code: " ANSI_COLOR_RED ANSI_COLOR_BOLD "[%ld]\n" ANSI_COLOR_RESET, line, res_code);
				}
				
			}
			sleep(0.9);
		}
	}
	
	
	curl_easy_cleanup(curl); // Invoke CURL to cleanup 

}

int main(int argc, char *argv[])
{
	char *result; 
	FILE *file; // Pointer too a FILE type
	char *baseurl = argv[1]; // Save The URL user entered into Base_url
	char *filename = argv[2]; // Save The file to use as wordlist
	int thread_count = atoi(argv[3]);
	int wc;
	char *scan_choice = argv[4];
	//printf("%s\n", baseurl);

	if (argc < 3) {
		printf("[" ANSI_COLOR_RED ANSI_COLOR_BOLD "!" ANSI_COLOR_RESET "] USAGE: %s <url> <wordlist> <thread Count> <scan type>\n", argv[0]); // if user didnt arguments exit
		printf("[" ANSI_COLOR_RED ANSI_COLOR_BOLD "!" ANSI_COLOR_RESET "] %s http://example.com words.txt 40, (subdir, subdomain)", argv[0]);
		return 1;
	}

	else if (thread_count > 35) {
		printf("[WARNING] Amount Of Threads Is Recommened To Not Go Over 35...\n");
		printf("[WARNING] Will Still Procced, CTRL+C To Exit\n");
	}

	else if ((check_if_up(baseurl)) == 0) { // Check If Site Is Reachable, if not exit program
		printf("[" ANSI_COLOR_RED ANSI_COLOR_BOLD "WARNING" ANSI_COLOR_RESET "] Website Isnt Reachable, Check URL!");
		return 1;
	}
	
	file = fopen(filename, "r"); // Open Wordlist In Read Mode
	if (file == NULL) {
		printf("\x1b[31m[!]\x1b[0m Failed Opening File!");
		return 1;
	}
	wc = count_words(file, result); // Count Amount of lines in file

	

	printf("\n");

	print_target_info(baseurl, scan_choice, filename, wc, thread_count);

	printf("\n\n");

	char *time; // Var To Hold Time thats returned From get_time()
	time = get_time();
	
	
	if (strcmp(scan_choice, "subdomain") == 0) {
		printf(ANSI_COLOR_BOLD "[!] Starting Scan At %s\n" ANSI_COLOR_RESET, time); // print Starting time
		printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "=============================================================\n" ANSI_COLOR_RESET);
		printf("||  [*] SUBDOMAIN %-20s\t[*] RESPONSE CODE  ||\n", " ");                                  // Just Some UI Stuff
		printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "=============================================================\n" ANSI_COLOR_RESET);

		pthread_t threads[thread_count];
		struct ThreadArgs args;
		args.file = file;
		args.baseurl = baseurl;

		for (int i = 0; i < thread_count; ++i) {
			pthread_create(&threads[i], NULL, subdomain_Scan, (void*)&args);
		}

		for (int i = 0; i < thread_count; ++i) {
			pthread_join(threads[i], NULL);
		}

		//subdomain_Scan(file, baseurl);
	}

	else if (strcmp(scan_choice, "subdir") == 0) {
		printf(ANSI_COLOR_BOLD "[!] Starting Scan At %s\n" ANSI_COLOR_RESET, time); // print Starting time
		printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "=============================================================\n" ANSI_COLOR_RESET);
		printf("||  [*] DIRECTORY %-20s\t[*] RESPONSE CODE  ||\n", " ");                                  // Just Some UI Stuff
		printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "=============================================================\n" ANSI_COLOR_RESET);

		pthread_t threads[thread_count];
		struct ThreadArgs args;
		args.file = file;
		args.baseurl = baseurl; 

		for (int i = 0; i < thread_count; ++i) {
			pthread_create(&threads[i], NULL, subdir_scan, (void*)&args);
		}

		for (int i = 0; i < thread_count; ++i) {
			pthread_join(threads[i], NULL);

		}

		//subdir_scan(file, baseurl); // Start Fuzzing
	}

	// pthread_t threads[thread_count];
	// struct ThreadArgs args;
	// args.file = file;
	// args.baseurl = baseurl; 

	// for (int i = 0; i < thread_count; ++i) {
	// 	pthread_create(&threads[i], NULL, subdir_scan, (void*)&args);
	// }

	// for (int i = 0; i < thread_count; ++i) {
	// 	pthread_join(threads[i], NULL);

	// }
	printf(ANSI_COLOR_BLUE ANSI_COLOR_BOLD "=============================================================\n" ANSI_COLOR_RESET);

	//subdir_scan(file, baseurl); // Start Fuzzing
	
	time = get_time(); // Get Time Again
	printf("[!] Scan Finished At %s\n", time); // Print End Time
	free(time);
	
	fclose(file); // Close File

	
	return 0;
}

