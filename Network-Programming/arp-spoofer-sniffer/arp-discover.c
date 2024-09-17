#include "header.h"
#include <math.h>

// ethernet mac address: ff:ff:ff:ff:ff:ff

// arp mac address 00:00:00:00:00:00
// arp ip address: 192.168.1.X
int sockFD;

void sig_handl(int sig) {
	system("clear && clear");
	close(sockFD);
	main();
}

int calc_cidr(char* str) {
	int amount;
	char* token;
	char* temp = strdup(str);

	token = strtok(temp, "/");
	token = strtok(NULL, "\n");
	int range = atoi(token);
	if(range < 0 || range > 32) {
		print_colors("Invalid CIDR Range", 1);
		free(temp);
		return -1;
	}

	double float_amount = pow(2, 32 - range) - 2;
	amount = (int)float_amount;
	free(temp);
	return amount;

}


int scan_network(char* ip_range) {
	int ip_nums;
	if(ip_range == NULL) {
		ip_nums = 50;
	} else {
		ip_nums = calc_cidr(ip_range);
		if(ip_nums < 0) {
			return -1;
		}
	}

	signal(SIGINT, sig_handl);
	void parse_packet(unsigned char* packet);
	unsigned char my_mac[HARDWARE_LENGTH] = {0x00, 0x0c, 0x29, 0xd9, 0x94, 0x21};
	unsigned char broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char holder[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char my_ip[PROTOCOL_LENGTH] = {192, 168, 1 , 31};
	unsigned char start_ip[PROTOCOL_LENGTH] = {192, 168, 1 , 1};
	unsigned char buffer[sizeof(struct ethhdr) + sizeof(struct arphdr)];
	struct ethhdr *ether_header = (struct ethhdr*)buffer;
	struct arphdr *arp_header = (struct arphdr*)(buffer + sizeof(struct ethhdr));

	struct sockaddr_ll sa;
	struct ifreq ifr;

	

	if((sockFD = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
		printf("Error: %s", strerror(errno));
		return 1;
	}

	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sockFD, SIOCGIFINDEX, &ifr) < 0) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sll_ifindex = ifr.ifr_ifindex;
	sa.sll_family = AF_PACKET;


	memcpy(ether_header->h_dest, broadcast, 6);
	memcpy(ether_header->h_source, my_mac, 6);
	ether_header->h_proto = htons(ARP_PROTOCOL);

	


	// uint8_t sender_hw_addr[HARDWARE_LENGTH]; 
    // uint8_t sender_proto_addr[PROTOCOL_LENGTH]; 
    // uint8_t target_hw_addr[HARDWARE_LENGTH]; 
    // uint8_t target_proto_addr[PROTOCOL_LENGTH]; 

	arp_header->hw_type = htons(1);
	arp_header->proto_type = htons(IPv4_PROTOCOL);
	arp_header->hw_len = 6;
	arp_header->proto_len = 4;
	arp_header->op = htons(1);
	memcpy(arp_header->sender_hw_addr, my_mac, 6);
	memcpy(arp_header->sender_proto_addr, my_ip, 4);
	memcpy(arp_header->target_hw_addr, holder, 6);

	printf("%s==================================================================================================%s\n", COLOR_1, RESET);
	printf("%s     IP %-30s MAC %-30s VENDOR%s\n", COLOR_2, " ", " ", RESET);
	printf("%s==================================================================================================%s\n", COLOR_3, RESET);

	for (int i = 0; i<ip_nums; i++) {
		start_ip[3] = i;
		memcpy(arp_header->target_proto_addr, start_ip, 4);


		if(sendto(sockFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
			printf("Error: %s\n", strerror(errno));
			return 1;
		}

		unsigned char recvbuffer[PACKET_SIZE];
		struct sockaddr_ll recv_sa;
		socklen_t sSize = sizeof(recv_sa);
		recvfrom(sockFD, recvbuffer, sizeof(recvbuffer), 0, (struct sockaddr*)&recv_sa, &sSize);

		
		parse_packet(recvbuffer);

		
	}

	
	

	return 0;



}

void parse_packet(unsigned char *packet) {
	char* get_ieee(unsigned char *mac);
	struct ethhdr *ether_header = (struct ethhdr*)packet;
	struct arphdr *arp_header = (struct arphdr*)(packet + sizeof(struct ethhdr));
	struct in_addr ia;
	char* ieee;
	
	

	if(ether_header->h_proto == ntohs(ARP_PROTOCOL)) {
		if (arp_header->op == ntohs(2)) {
			memcpy(&ia, arp_header->sender_proto_addr, sizeof(ia));
			ieee = get_ieee(arp_header->sender_hw_addr);
			// char fpBuffer[400];
			// snprintf(fpBuffer, sizeof(fpBuffer), "%s %02x:%02x:%02x:%02x:%02x:%02x\n", inet_ntoa(ia), arp_header->sender_hw_addr[0], arp_header->sender_hw_addr[1], arp_header->sender_hw_addr[2], arp_header->sender_hw_addr[3], arp_header->sender_hw_addr[4],arp_header->sender_hw_addr[5]);
			// fwrite(fpBuffer, sizeof(char), strlen(fpBuffer), fp);
			// memset(fpBuffer, 0, sizeof(fpBuffer));
			printf("%-30s %02x:%02x:%02x:%02x:%02x:%02x                       %s \n", inet_ntoa(ia), arp_header->sender_hw_addr[0],
																		 arp_header->sender_hw_addr[1],
																		 arp_header->sender_hw_addr[2],
																		 arp_header->sender_hw_addr[3],
																		 arp_header->sender_hw_addr[4],
																		 arp_header->sender_hw_addr[5], ieee);
		}
	} 
	else {
		printf("fail");
		return;
	}
}




char* get_ieee(uint8_t *mac) {
	unsigned int octect1, octect2, octect3;
	char buffer[10];
	octect1 = mac[0];
	octect2 = mac[1];
	octect3 = mac[2];
	snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x", octect1, octect2, octect3);
	//printf("%s\n", buffer);

	FILE* fp;

	fp = fopen("files/iiee", "r");
	char* manufac = NULL;
	char* fail = "Unknown Vendor";

	if(fp == NULL) {
		return NULL;
	}

	char file_buffer[100];
	while(fgets(file_buffer, sizeof(file_buffer), fp) != NULL) {
		char* token = strtok(file_buffer, " ");
		if (token && strcmp(token, buffer) == 0) {
			manufac = strtok(NULL, "\n");
			return manufac;
		}
	
	}

	return fail;

}

// target 192.168.1.23 192.168.1.1
