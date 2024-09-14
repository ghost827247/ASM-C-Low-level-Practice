#include "header.h"
//sudo iptables -A FORWARD -i eth0 -j ACCEPT
//sudo iptables -A FORWARD -o eth0 -j ACCEPT


char *ip = NULL;
char *router_ip = NULL;
char *tMac = NULL;
char *rMac = NULL;
pid_t spoofer_pid = -1;
pid_t sniifer_pid = -1;
void print_line_with_color(const char* line, int color_code) {
    printf("\033[38;5;%dm%s\033[0m\n", color_code, line);
}
int colors[] = {
        18, 19, 20, 21, 26, 27, 28, 33, 39, 44, 45, 46, 47
};


void print_colors(char* str, int okay_or_error) {
	if (okay_or_error == 0) {
		printf("%s[%s*%s%s]%s %s\n", BOLD, TEXT_GREEN, RESET, BOLD, RESET, str);
	} else {
		printf("%s[%s!%s%s]%s %s\n", BOLD, TEXT_RED, RESET, BOLD, RESET, str);
	}
	
}

void print_banner(int choice) {

	if(choice == 1) {
		// UI so ugly fix this shit
		const char* lines[] = {
	        "+========================================================================+",
	        "|    _____ _               _                       ___  ____________     |",
	        "|   /  ___| |             | |                     / _ \\ | ___ \\ ___ \\    |",
	        "|   \\ `--.| |__   __ _  __| | _____      ________/ /_\\ \\| |_/ / |_/ /    |",
	        "|    `--. \\ '_ \\ / _` |/ _` |/ _ \\ \\ /\\ / /______|  _  ||    /|  __/     |",
	        "|   /\\__/ / | | | (_| | (_| | (_) \\ V  V /       | | | || |\\ \\| |        |",
	        "|   \\____/|_| |_|\\__,_|\\__,_|\\___/ \\_/\\_/        \\_| |_/\\_| \\_\\_|        |",
	        "|                        Created By Zevuxo                               |",
	        "+=========================================================================+"
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
		print_colors("USAGE", 0);
		printf("%s|----%s[%s%s+%s]%s Scan%s: Discover Hosts On the Network Using ARP\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Target%s <IP> <GATEWAY>: Specify Which Computer We Want to Poison the ARP Entry's Of\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Spoof%s: Start ARP Spoofing\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Sniff%s: Start Sniffing Network Traffic\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Stop <choice>%s: Choose Process To Stop, spoof or sniff\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Clear%s: Clear Screen\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s Banner%s: Print Banner\n",BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);
		printf("%s|----%s[%s%s+%s]%s ?%s: Display This Help Message\n\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET);



	}

}

int find_mac(char* ip, FILE *fp, int which_mac) {
	char strBuffer[50];
	while(fgets(strBuffer, sizeof(strBuffer), fp) != NULL) {
		char* token = strtok(strBuffer, " ");
		if (strcmp(ip, token) == 0) {
			if( which_mac == 1) {
				tMac = strdup(strtok(NULL, "\n"));
			} else {
				rMac = strdup(strtok(NULL, "\n"));
			}
			rewind(fp);
			break;
		}
	}
	if (tMac == NULL) {
		return 1;
	} else if (rMac == NULL) {
		return 2;
	} else {
		return 0;
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
		FILE* fp;
		char* token = strtok(str, " ");
		ip = strtok(NULL, " ");
		router_ip = strtok(NULL, "\n");

		fp = fopen("files/macs.txt", "r");
		if(fp == NULL) {
			printf("Failed Opening Mac File\n");
			return;
		}
		int check;
		char strBuffer[50];
		check = find_mac(ip, fp, 1);
		if (check == 1) {
			printf("[-] Failed Getting Target MAC, Does The IP Exist?\n"); return;
		}
		check = find_mac(router_ip, fp, 0);
		if (check == 2) {
			printf("[-] Failed Getting Routers MAC, Does The IP Exist?\n"); return;
		}


		print_colors("Target Details", 0);
		printf("%s|----%s[%s%s+%s]%s Target IP%s: %s\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET, ip);
		printf("%s|----%s[%s%s+%s]%s Target MAC%s: %s\n",BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET, tMac);
		printf("|\n");
		printf("%s|----%s[%s%s+%s]%s Gateway IP%s: %s\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET, router_ip);
		printf("%s|----%s[%s%s+%s]%s Gateway MAC%s: %s\n", BOLD, RESET, BOLD, TEXT_BLUE, RESET, BOLD, RESET, rMac);
	}

	else if(strcmp(str, "spoof") == 0) {
		if (ip == NULL || router_ip == NULL || tMac == NULL || rMac == NULL) {
			print_banner(2);
			print_colors("Enter Target Details First", 1);
			return;
		} else {
			pid_t pID = fork();
			if(pID == 0) {

				entry(ip, router_ip, tMac, rMac);

				exit(0);
			}
			else if (pID < 0) {
				print_colors("Failed Starting Spoofer", 1);
			}
			else {
				spoofer_pid = pID;
				print_colors("Spoofer Started!", 0);
			}
			// entry(ip, router_ip, tMac, rMac);
		}
		
	}

	// Handle Command to Stop Spoofer Or Sniffer
	else if(strncmp(str, "stop", 4) == 0) {
		char* token = strtok(str, " ");

		if (token == NULL) {
			print_colors("Must Specify What to Stop", 1);
		}

		token = strtok(NULL, "\n");

		if(strcmp(token, "spoofer") == 0) {
			if (spoofer_pid > 0) {
				if(kill(spoofer_pid, SIGTERM) == 0) {
					print_colors("Spoofer Stopped", 0);
					spoofer_pid = -1;
				} else {
					print_colors("Failed To Stop Spoofer", 1);
				}
			} else {
				print_colors("Spoofer Isnt Running", 1);
			}
		}
		else if(strcmp(token, "sniffer") == 0) {
			if(sniifer_pid > 0) {
				if(kill(sniifer_pid, SIGTERM) == 0) {
					print_colors("Sniffer Stopped", 0);
					sniifer_pid = -1;
				} else {
					print_colors("Failed Stopping Sniffer", 1);
				}
			} else {
				print_colors("Sniffer Isnt Running", 1);
			}
		}
		
		
	}

	// TODO Create Sniffer.c and parse this command
	else if (strcmp(str, "sniff") == 0) {
		if (ip == NULL || router_ip == NULL) {
			printf("[-] Enter Target Details First\n");
		}
		printf("Make This Nigga\n");
	}
 
 	// Below Functions Are obvious
	else if (strcmp(str, "exit") == 0 || strcmp(str, "quit") == 0) {
		if (spoofer_pid > 0) {
			if(kill(spoofer_pid, SIGTERM) == 0) {
				exit(0);
			} else {
				print_colors("Failed Stopping Spoofer, Try Again", 1);
			}
		} 
		

		else if (sniifer_pid > 0) {
			if(kill(sniifer_pid, SIGTERM) == 0) {
				exit(0);
			} else {
				print_colors("Failed Stopping Sniffer, Try Again", 1);
			}
		} 
		else {
			exit(0);
		}
		
	} 

	else if (strcmp(str, "clear") == 0 || strcmp(str, "cls") == 0) {
		system("clear");

	}

	else if (strcmp(str, "?") == 0) {
		print_banner(2);
	}

	else if (strcmp(str, "banner") == 0) {
		print_banner(1);
	}
	else {
		print_colors("Invalid Command", 1);
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
