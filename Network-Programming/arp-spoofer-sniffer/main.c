#include "header.h"


char *ip = NULL;
char *router_ip = NULL;
void print_line_with_color(const char* line, int color_code) {
    printf("\033[38;5;%dm%s\033[0m\n", color_code, line);
}
int colors[] = {
        18, 19, 20, 21, 26, 27, 28, 33, 39, 44, 45, 46, 47
};

void print_banner(int choice) {

	if(choice == 1) {
		// UI so ugly fix this shit
		const char* lines[] = {
	        "+=======================================================+",
	        "|    ___                  ___  ________ ________  ___   |",
	        "|   / _ \\                 |  \\/  |_   _|_   _|  \\/  |   |",
	        "|  / /_\\ \\_ __ _ __ ______| .  . | | |   | | | .  . |   |",
	        "|  |  _  | '__| '_ \\______| |\\/| | | |   | | | |\\/| |   |",
	        "|  | | | | |  | |_) |     | |  | |_| |_  | | | |  | |   |",
	        "|  \\_| |_/_|  | .__/      \\_|  |_/\\___/  \\_/ \\_|  |_/   |",
	        "|             | |                                       |",
	        "|             |_|                                       |",
	        "|                        Created By Zevuxo              |",
	        "+=======================================================+"
	    };

	    int line_num = sizeof(lines) / sizeof(lines[0]);
	    int color_num = sizeof(colors) / sizeof(colors[0]);

	    for (int i = 0; i < line_num; i++) {
	        int color_index = (i * color_num) / line_num;
	        print_line_with_color(lines[i], colors[color_index]);
	    }

	}
	else if (choice == 2) {
		// Straight up skid ddos UI
		printf("[*] USAGE\n");
		printf("%s|----%s[+] Scan: %sDiscover Hosts On the Network Using ARP%s\n", BOLD, RESET, BOLD, RESET);
		printf("%s|----%s[+] Target <IP> <GATEWAY>: %sSpecify Which Computer We Want to Poison the ARP Entry's Of%s\n", BOLD, RESET,BOLD, RESET);
		printf("%s|----%s[+] Spoof: %sStart ARP Spoofing%s\n", BOLD, RESET,BOLD, RESET);
		printf("%s|----%s[+] Sniff: %sStart Sniffing Network Traffic%s\n", BOLD, RESET,BOLD, RESET);
		printf("%s|----%s[+] Clear: %sClear Screen%s\n", BOLD, RESET, BOLD, RESET);
		printf("%s|----%s[+] Banner: %sPrint Banner%s\n", BOLD, RESET, BOLD, RESET);
		printf("%s|----%s[+] ?: %sDisplay This Help Message%s\n\n", BOLD, RESET, BOLD, RESET);



	}

}

void compare_choice(char* str) {

	size_t str_len = strlen(str);

	for (int i = 0; i < str_len; i++) {
		str[i] = tolower(str[i]);
	}

	if (strcmp(str, "scan") == 0) {
		scan_network();
	}

	else if (strncmp(str, "target", 6) == 0) {
		char* token = strtok(str, " ");
		ip = strtok(NULL, " ");
		router_ip = strtok(NULL, "\n");
		printf("[*] Target: %s\n", ip);
		printf("[*] Gateway: %s\n", router_ip);
	}

	else if(strcmp(str, "spoof") == 0) {
		if (ip == NULL || router_ip == NULL) {
			print_banner(2);
			printf("[-] Enter Target Details First\n");
			return;
		}
		entry(ip, router_ip);
	}

	else if (strcmp(str, "sniff") == 0) {
		if (ip == NULL || router_ip == NULL) {
			printf("[-] Enter Target Details First\n");
		}
		printf("Make This Nigga\n");
	}
 
	else if (strcmp(str, "exit") == 0) {
		exit(1);
	} 

	else if (strcmp(str, "clear") == 0) {
		system("clear");

	}

	else if (strcmp(str, "?") == 0) {
		print_banner(2);
	}

	else if (strcmp(str, "banner") == 0) {
		print_banner(1);
	}
	else {
		printf("[-] Invalid Command\n");
	}

}


int main(void)
{
	char choice[50];
	print_banner(1);
	print_banner(2);
	while(1) {
		printf("root@ARP-SPOOF $> ");
		fgets(choice, sizeof(choice), stdin);
		choice[strcspn(choice, "\n")] = '\0';

		compare_choice(choice);


	}

	return 0;
}
