#include "header.h"
//#define PACKET_SIZE 65535

unsigned char test_mac[6] = {0xd0, 0x39, 0x57,0xe3, 0x80,0xa9};


//void check_for_ip_tcp()




//void handle_arp(unsigned char* buffer)


// void handle_http(unsigned char* buffer)

void forNow(unsigned char* buffer) {
	struct ethhdr *eth_header = (struct ethhdr*)buffer;

	if(memcmp(eth_header->h_source, buffer, 6) == 0 || memcmp(eth_header->h_dest, buffer, 6) == 0) {
		printf("True");
	}
}

int main() {
	unsigned char *buffer = (unsigned char*)malloc(65535);
	int sockFD;

	struct sockaddr sa;
	socklen_t sSize = sizeof(sa);


	if((sockFD = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0 ){
		printf("[!] Error: Are you Running As Root\n");
		return -1;
	}

	if(sockFD) {
		while(1) {
			recvfrom(sockFD, buffer, sizeof(buffer), 0, &sa, &sSize);


		}
	}

	return 0;
}
