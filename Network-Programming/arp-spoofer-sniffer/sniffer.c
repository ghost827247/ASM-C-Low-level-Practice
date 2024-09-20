#include "header.h"
#define TEXT_GREEN      "\033[32m"
#define RESET           "\033[0m"
#define BOLD            "\033[1m"
#define TEXT_BLUE       "\033[34m"

int last_was_icmp = 0;

// struct arphdr {
//     uint16_t hw_type;         
//     uint16_t proto_type;      
//     uint8_t hw_len;            
//     uint8_t proto_len;        
//     uint16_t op;               
//     uint8_t sender_hw_addr[6]; 
//     uint8_t sender_proto_addr[4]; 
//     uint8_t target_hw_addr[6]; 
//     uint8_t target_proto_addr[4]; 
// };


void format_mac(const uint8_t* mac, char* str) {
	sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void handle_arp(unsigned char* buffer, struct arphdr* arp_hdr) {
	char sender_ip[INET_ADDRSTRLEN];
	char target_ip[INET_ADDRSTRLEN];
	char sender_mac[18];
	char target_mac[18];
	
	inet_ntop(AF_INET, arp_hdr->sender_proto_addr, sender_ip, sizeof(sender_ip));
	inet_ntop(AF_INET, arp_hdr->target_proto_addr, target_ip, sizeof(target_ip));
	format_mac(arp_hdr->sender_hw_addr, sender_mac);
	format_mac(arp_hdr->target_hw_addr, target_mac);

	last_was_icmp = 1;

	if(ntohs(arp_hdr->op) == 1) {
		printf("%s%sARP%s: %-13s | Source IP: %-10s | Source Mac: %-10s | Target IP: %-10s %-6s\n", BOLD, TEXT_GREEN, RESET, "Type: Request", sender_ip, sender_mac, target_ip, "|");
	} else if(ntohs(arp_hdr->op) == 2) {
		printf("%s%sARP%s: %-10s | Source IP: %-10s | Source Mac: %-10s | Target IP: %-10s %-2s\n Target Mac: %-10s\n", BOLD, TEXT_GREEN, RESET, "Type: Reply", sender_ip, sender_mac, target_ip, target_mac, "|");
	}
}


int hanlde_icmp(unsigned char* packet) {
	// Type 0 – Echo reply
	// Type 3 – Destination unreachable
    // Type 5 – Redirect Message
	// Type 8 – Echo Request
	// Type 11 – Time Exceeded
	// Type 12 – Parameter problem



	struct iphdr *ip_hdr = (struct iphdr*)(packet + sizeof(struct ethhdr));
	struct icmphdr *icmp_hdr = (struct icmphdr*)(packet + sizeof(struct ethhdr) + sizeof(struct iphdr));
	struct in_addr source, dest;

	memset(&source, 0, sizeof(source));
	memset(&dest, 0, sizeof(dest));


	source.s_addr = ip_hdr->saddr;
	dest.s_addr = ip_hdr->daddr;
	int type = (unsigned int)icmp_hdr->type;

	if(last_was_icmp) {
		printf("\n");
		last_was_icmp = 0;
	}


	if(type == 8) {
		printf("%s%sICMP%s %-20s | Source: %-15s | Destination: %-15s | ip_seq: %-5d | ttl: %-5u %-5s\n", BOLD, TEXT_BLUE, RESET, "Type: Echo Request", 
																											inet_ntoa(source), 
																											inet_ntoa(dest), 
																											(unsigned int)ntohs(icmp_hdr->un.echo.sequence), (unsigned int)ip_hdr->ttl, "|");
	}
	else if (type == 0) {
		printf("%s%sICMP%s %-20s | Source: %-15s | Destination: %-15s | ip_seq: %-5d | ttl: %-5u %-5s\n\n", BOLD, TEXT_BLUE, RESET, "Type: Echo Reply", 
																											inet_ntoa(source), inet_ntoa(dest), 
																											(unsigned int)ntohs(icmp_hdr->un.echo.sequence),
																											(unsigned int)ip_hdr->ttl, "|");

	}
	
}

void handle_ipv4(unsigned char* buffer, struct iphdr *ip_hdr) {
	// 1 == icmp
	// 6 == tcp
	// 7 == udp
	if(ip_hdr->protocol == 1) {
		hanlde_icmp(buffer);
	}
}

void parse(unsigned char* packet) {
	struct ethhdr *eth_header = (struct ethhdr*)packet;
	
	if (ntohs(eth_header->h_proto) == 0x0806) {
		struct arphdr *arp_header = (struct arphdr*)(packet + sizeof(struct ethhdr));
		handle_arp(packet, arp_header);
	} 
	else if (ntohs(eth_header->h_proto) == 0x0800) {
		struct iphdr *ip_header = (struct iphdr*)(packet + sizeof(struct ethhdr));
		handle_ipv4(packet, ip_header);
	}
}

int sniffer_entry(void)
{	
	unsigned char buffer[65536];
	int sockFD;
	struct sockaddr sa;
	socklen_t sSize = sizeof(sa);

	if((sockFD = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("[!] Error: %s\n", strerror(errno));
		return 1;
	}

	while(1) {
		recvfrom(sockFD, buffer, sizeof(buffer), 0, &sa, &sSize);
		parse(buffer);
	}
	

	close(sockFD);

	return 0;
}
