#include "header.h"

// ethernet mac address: ff:ff:ff:ff:ff:ff

// arp mac address 00:00:00:00:00:00
// arp ip address: 192.168.1.X

int scan_network() {
	unsigned char my_mac[HARDWARE_LENGTH] = {0x00, 0x0c, 0x29, 0xd9, 0x94, 0x21};
	unsigned char broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char holder[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char my_ip[PROTOCOL_LENGTH] = {192, 168, 1 , 31};
	unsigned char my_ip2[PROTOCOL_LENGTH] = {192, 168, 1 , 23};
	unsigned char buffer[sizeof(struct ethhdr) + sizeof(struct arphdr)];
	struct ethhdr *ether_header = (struct ethhdr*)buffer;
	struct arphdr *arp_header = (struct arphdr*)(buffer + sizeof(struct ethhdr));

	struct sockaddr_ll sa;
	struct ifreq ifr;

	int sockFD;

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

	info("Creating Ethernet Header...");

	memcpy(ether_header->h_dest, broadcast, 6);
	memcpy(ether_header->h_source, my_mac, 6);
	ether_header->h_proto = htons(ARP_PROTOCOL);

	okay("Done!\n");

	info("Creating ARP Request...");

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
	memcpy(arp_header->target_proto_addr, my_ip2, 4);

	if(sendto(sockFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	}

	char recvbuffer[PACKET_SIZE];
	struct sockaddr_ll recv_sa;
	socklen_t sSize = sizeof(recv_sa);
	recvfrom(sockFD, recvbuffer, sizeof(recvbuffer), 0, (struct sockaddr*)&recv_sa, &sSize);

	parse_packet(recvbuffer);

	return 0;



}

void parse_packet(unsigned char *packet) {

}

int main(int argc, char* argv[]) {
	scan_network();
	return 0;
}
