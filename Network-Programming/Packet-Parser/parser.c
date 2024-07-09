#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <netinet/ether.h>
#include <net/ethernet.h> // For ETH_P_ALL
#include <netpacket/packet.h>
#include <linux/tcp.h>




// uint8_t 	h_dest [ETH_ALEN]
 	// Destination MAC address. More...
 
// uint8_t 	h_source [ETH_ALEN]
 	// Source MAC address. More...
 
// uint16_t 	h_protocol
 	// Protocol ID. More...
 


void parse_ether(unsigned char* buffer) {
	struct ethhdr *eth_header = (struct ethhdr*)(buffer);
    
    	// Print source MAC address
    	printf("+------------------------------------+\n");
	printf("|              ETHER HEADER          |\n");
	printf("+------------------------------------+\n");
	printf("|-%-16s %02x:%02x:%02x:%02x:%02x:%02x |\n", "SOURCE-MAC:", eth_header->h_source[0], eth_header->h_source[1], eth_header->h_source[2], eth_header->h_source[3], eth_header->h_source[4], eth_header->h_source[5]);
	printf("|-%-16s %02x:%02x:%02x:%02x:%02x:%02x |\n", "DESTIN-MAC:", eth_header->h_dest[0], eth_header->h_dest[1], eth_header->h_dest[2], eth_header->h_dest[3], eth_header->h_dest[4], eth_header->h_dest[5]);
	printf("|-%-28s %-5d |\n", "PROTOCOL:", ntohs(eth_header->h_proto));

}	
//printf("|-%s %-5d |\n", "PROTO:", eth_header->h_proto);

void Parse_ip(unsigned char* buffer) {

// ROW 1
	// VERSION         --> 4 bits (higher)
	// HEADER LENGTH   --> 4 bits (lower)
	// TYPE OF SERVICE --> 16 bits
    // TOTAL LENGTH    --> 16 bits

// ROW 2
	// IDENTIFICATION  --> 16 bits
	// IP FLAGS        -->
	// FRAGMENT OFFSET --> 16 bits

// ROW 3
	// TIME TO LIVE    --> 8 bits
	// PROTOCOL		   --> 8 bits
	// CHECKSUM        --> 16 bits

// ROW 4
	// SOURCE ADDRESS  --> 32 bits

// ROW 5
	// DEST ADDRESS    --> 32 bits

// ROW 6
	// OPTIONS/PADDING --> 32 bits
	



	//struct ethhdr *eth = (struct ethhdr *)buffer; // Declare the Ethernet Header Struct so we can skip it
    	struct iphdr *ip_address = (struct iphdr*)(buffer + sizeof(struct ethhdr)); // We Need To access the IP Header, Since RAW Sockets Captures the entire packet
    																			// We Need to Skip To The IP Header by getting the buffer then adding the sizeof ethernet struct in bytes 
	struct sockaddr_in source, dest;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = ip_address->saddr;
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = ip_address->daddr;
	int spaces = 25;

	printf("+------------------------------------------+\n");
	printf("|                  IP HEADER               |\n");
	printf("+------------------------------------------+\n");
	printf("|-%-30s %-5d     |\n", "IP-VERSION:", (unsigned int)ip_address->version);
	printf("|-%-30s %-5d     |\n", "HEADER-LENGTH:", (unsigned int)ip_address->ihl);
	printf("|-%-30s %-5d     |\n", "TYPE-OF-SERVICE:", (unsigned int)ip_address->tos);
	printf("|-%-30s %-5d     |\n", "TOTAL-LENGTH:", ntohs(ip_address->tot_len));
	printf("|-%-30s %-5d     |\n", "IDENTIFICATION: ", ntohs(ip_address->id));
	printf("|-%-30s %-5d     |\n", "FLAGS:", (unsigned int)ip_address->tos);
	printf("|-%-30s %-5d     |\n", "FRAGMENT-OFFEST:", (unsigned int)ip_address->frag_off);
	printf("|-%-30s %-5d     |\n", "TIME-TO-LIVE:", (unsigned int)ip_address->ttl);
	printf("|-%-30s %-5d     |\n", "PROTOCOL:", (unsigned int)ip_address->protocol);
	printf("|-%-30s %-5d     |\n", "CHECK-SUM:", ntohs(ip_address->check));
	printf("|-%-27s %s  |\n", "SOURCE:", inet_ntoa(source.sin_addr));
	printf("|-%-27s %s |\n", "DESTINATION:", inet_ntoa(dest.sin_addr));



	//printf("| HEADER-LENGTH: %d%*s|\n", (unsigned int)ip_address->ihl, spaces, " ");
	//printf("| TYPE-OF-SERVICE: %d%*s|\n", (unsigned int)ip_address->tos, spaces, " ");
	//printf("| TOTAL-LENGTH: %d               |\n", ntohs(ip_address->tot_len));
	//printf("| IDENTIFICATION: %d          |\n", (unsigned int)ip_address->id);
	//printf("| FLAGS: GOOGLE THIS             |\n");
	//printf("| FRAGMENT-OFFEST: %d            |\n", (unsigned int)ip_address->frag_off);
	//printf("| TIME-TO-LIVE: %d               |\n", (unsigned int)ip_address->ttl);
	//printf("| PROTOCOL: %d                    |\n", (unsigned int)ip_address->protocol);
	//printf("| CHECK-SUM: %d               |\n", (unsigned int)ip_address->check);
	//printf("| SOURCE: %s      |\n", inet_ntoa(source.sin_addr));
	//printf("| DESTINATION: %s |\n", inet_ntoa(dest.sin_addr));


}


 void parse_transport(unsigned char* buffer) {

 // 	struct tcphdr {
   //     __be16  source;
   //     __be16  dest;
   //     __be32  seq;
   //     __be32  ack_seq;
   // #if defined(__LITTLE_ENDIAN_BITFIELD)
   //     __u16   res1:4,
   //         doff:4,
   //         fin:1,
   //         syn:1,
   //         rst:1,
   //         psh:1,
   //         ack:1,
   //         urg:1,
   //         ece:1,
   //         cwr:1;
   // #elif defined(__BIG_ENDIAN_BITFIELD)
   //     __u16   doff:4,
   //         res1:4,
   //         cwr:1,
   //         ece:1,
   //         urg:1,
   //         ack:1,
   //         psh:1,
   //         rst:1,
   //         syn:1,
   //         fin:1;
   // #else
   // #endif  
   //     __be16  window;
   //     __sum16 check;
   //     __be16  urg_ptr;
   //};


 	struct tcphdr *tcp_header = (struct tcphdr*)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
	printf("+-----------------------------------------------+\n");
	printf("|                 TRANSPORT HEADER              |\n");
	printf("+-----------------------------------------------+\n");
	printf("|-%-30s %-5d          |\n", "SOURCE-PORT:", ntohs(tcp_header->source));
	printf("|-%-30s %-5d          |\n", "DEST-PORT:", ntohs(tcp_header->dest));
	printf("|-%-24s %-5u           |\n", "SEQUENCE:", ntohl(tcp_header->seq));
	printf("|-%-24s %-5u           |\n", "ACK-NUMBER:", ntohl(tcp_header->ack_seq));
	printf("|-%-30s %-5d          |\n", "HEADER-LENGTH:", (unsigned int)tcp_header->doff*4);
	printf("\\-----------------------------------------------+\n");
	printf(" \\-%-21s %-5d                  |\n", "URG:", (unsigned int)tcp_header->urg);
	printf("  |-%-20s %-5d                  |\n", "ACK:", (unsigned int)tcp_header->ack);
	printf("  |-%-20s %-5d                  |\n", "PSH:", (unsigned int)tcp_header->psh);
	printf("  |-%-20s %-5d                  |\n", "RST:", (unsigned int)tcp_header->rst);
	printf("  |-%-20s %-5d                  |\n", "SYN:", (unsigned int)tcp_header->syn);
	printf(" /-%-21s %-5d                  |\n", "FIN", (unsigned int)tcp_header->fin);
	printf("/-----------------------------------------------+\n");


 }



void print_data(unsigned char* buffer, int buf_len) {
    // Calculate start of payload
    unsigned char* payload = buffer + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr);
    
    // Print payload in hexadecimal format
    printf("Payload Data:\n");
    for (int i = 0; i < buf_len - (payload - buffer); i++) {
        printf("%02x ", payload[i]);
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
	struct sockaddr sAddr;
	socklen_t sSize = sizeof(sAddr);

	int sock;
	int sAddr_size, data_size;
	unsigned char *buffer = (unsigned char*)malloc(65535);


	if ((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0) {
		printf("Failed Making Socket");
		return 1;
	}

	printf("Socket FD: %d\n", sock);

	while(1) {
		if ((data_size = recvfrom(sock, buffer, 65535, 0, &sAddr, &sSize)) < 0) {
			printf("Failed Caputring: %s", strerror(errno));
			break;
		}
		printf("Data size: %d\n", data_size);
		printf("Parsing Data Link Header\n");
		parse_ether(buffer);
		Parse_ip(buffer);
		parse_transport(buffer);
		print_data(buffer, data_size);
		printf("\n\n\n\n");
	}

	return 0;
}
