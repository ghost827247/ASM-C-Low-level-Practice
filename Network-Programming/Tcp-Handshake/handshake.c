#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

struct psuedo_header {
	u_int32_t source_addr;
	u_int32_t dest_addr;
	u_int8_t  placeholder;
	u_int8_t  protocol;
	u_int16_t tcp_length;
};

#define PACKET_LEN 8000
#define OPT_LEN 20

unsigned short checksum_calc(const char *buf, unsigned size) {
	unsigned sum = 0, i;
	for (i = 0; i < size -1; i+=2) {
		unsigned short word16 = *(unsigned short*)&buf[i];
		sum += word16;
	}

	if (size & 1) {
		unsigned short word16 = (unsigned char)buf[i];
		sum += word16;
	}

	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return ~sum;
}

void syn_packet(struct sockaddr_in* source, struct sockaddr_in* dest, char** packet_out, int* packet_out_len) {
	// Allocate memory and zero it out
	char *buffer = calloc(PACKET_LEN, sizeof(char));

	struct iphdr *ip_header = (struct iphdr*)buffer;
	struct tcphdr *tcp_header = (struct tcphdr*)(buffer + sizeof(struct iphdr));
	struct psuedo_header psh;

// IP HEADER CONFIURATION
	// Header Length 5: 32 bit = 1 bytes, 5 * 4 = 40 bytes 
	ip_header->ihl = 5;

	// Ip Version: 4 = IPv4
	ip_header->version = 4;

	// Type Of Service: 0 = default priority
	ip_header->tos = 0;

	// Total-Length Including Header and data
	ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + OPT_LEN;
	
	// ID Of this Packet: Just Get Random Value
	ip_header->id = htonl(rand() % 65535);
	
	// Do we Fragment? IP_DF is a macro for 0 = dont fragment
	ip_header->frag_off = htons(IP_DF);
	
	// Time To Live: 64 = 64 hops till 0 which then it will be dropped
	ip_header->ttl = 64;
	
	// Protocol: We Are Using TCP
	ip_header->protocol = IPPROTO_TCP;
	
	// Checksum: set to 0 intially, we will set it later
	ip_header->check = 0;
	
	// Source Address: From Us
	ip_header->saddr = source->sin_addr.s_addr;
	
	// Destination Address: My Router
	ip_header->daddr = dest->sin_addr.s_addr;

// TCP PACKET CONFIG. WE NEED SYN = 1
	// Source Port: My Port 
	tcp_header->source = source->sin_port;
	// Dest Port, Routers Port 80
	tcp_header->dest = dest->sin_port;
	// Sequence Number: Just Grab rand Value
	tcp_header->seq = htonl(123456);
	// Ack sequance Number: 0 = first packet
	tcp_header->ack_seq = htonl(0);
	// size of TCP Header: 40 bytes, 10 * 4
	tcp_header->doff = 10;
	// FIN Flag: 0 SYN Packet
	tcp_header->fin = 0;
	// Urgent Flag: 0 SYN Packet
	tcp_header->urg = 0;
	// PUSH flag: 0
	tcp_header->psh = 0;
	// SYN Flag: 1(This Is the SYN in the TCP Handshake)
	tcp_header->syn = 1;
	// ACK: 0
	tcp_header->ack = 0;
	tcp_header->rst = 0;
	// Checksum: set to 0  for now
	tcp_header->check = 0;
	// Windows Size(How Much Data Can Be Sent at once)
	tcp_header->window = htons(5840);
	tcp_header->urg_ptr = 0;


	psh.source_addr =  source->sin_addr.s_addr;
	psh.dest_addr = dest->sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcphdr) + OPT_LEN);
	int psize = sizeof(struct psuedo_header) + sizeof(struct tcphdr) + OPT_LEN;
	char* pshGram = malloc(psize);

	memcpy(pshGram, (char*)&psh, sizeof(struct psuedo_header));
	memcpy(pshGram + sizeof(struct psuedo_header), tcp_header, sizeof(struct tcphdr) + OPT_LEN);

	// SET MSS
	buffer[40] = 0x02;
	buffer[41] = 0x04;
	int16_t mss = htons(48);
	memcpy(buffer + 42, &mss, sizeof(int16_t));
	// ENABLE SACK(selective acknowledgement)
	buffer[44] = 0x04;
	buffer[45] = 0x02;
	// Do the same for the PseudoGram
	pshGram[32] = 0x02;
	pshGram[33] = 0x04;
	memcpy(pshGram + 34, &mss, sizeof(int16_t));
	pshGram[36] = 0x04;
	pshGram[37] = 0x02;

	tcp_header->check = checksum_calc((const char*)pshGram, psize);
	ip_header->check = checksum_calc((const char*)buffer, ip_header->tot_len);

	*packet_out = buffer;
	*packet_out_len = ip_header->tot_len;
	free(pshGram);


}


void send_ack(struct sockaddr_in* src, struct sockaddr_in* dst, char** packet_out, int* pack_len, int32_t seq, int32_t ack) {
	char* segment = calloc(PACKET_LEN, sizeof(char));

	struct iphdr *ip_header2   = (struct iphdr*)segment;
	struct tcphdr *tcp_header2 = (struct tcphdr*)(segment + sizeof(struct iphdr));
	struct psuedo_header psh;

// IP HEADER CONFIG
	ip_header2->ihl      = 5;
	ip_header2->version  = 4;
	ip_header2->tos      = 0;
	ip_header2->tot_len  = sizeof(struct iphdr) + sizeof(struct tcphdr) + OPT_LEN;
	ip_header2->id       = htons(rand() % 65535);
	ip_header2->frag_off = 0;
	ip_header2->ttl      = 64;
	ip_header2->protocol = IPPROTO_TCP;
	ip_header2->check    = 0;
	ip_header2->saddr    = src->sin_addr.s_addr;
	ip_header2->daddr    = dst->sin_addr.s_addr;


// TCP HEADER CONFIG
	tcp_header2->source  = src->sin_port;
	tcp_header2->dest    = dst->sin_port;
	tcp_header2->seq     = htonl(seq);
	tcp_header2->ack_seq = htonl(ack);
	tcp_header2->doff    = 10;
	tcp_header2->fin     = 0;
	tcp_header2->urg     = 0;
	tcp_header2->rst     = 0;
	tcp_header2->ack     = 1;
	tcp_header2->syn     = 0;
	tcp_header2->psh     = 0;
	tcp_header2->check   = 0;
	tcp_header2->window  = htons(5480);
	tcp_header2->urg_ptr = 0;

	psh.source_addr = src->sin_addr.s_addr;
	psh.dest_addr   = dst->sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol    = IPPROTO_TCP;
	psh.tcp_length  = htons(sizeof(struct tcphdr) + OPT_LEN);
	int pSize       = sizeof(struct psuedo_header) + sizeof(struct tcphdr) + OPT_LEN;

	char* pshGram = malloc(pSize);
	memcpy(pshGram, (char*)&psh, sizeof(struct psuedo_header));
	memcpy(pshGram + sizeof(struct psuedo_header), tcp_header2, sizeof(struct tcphdr) + OPT_LEN);

	tcp_header2->check = checksum_calc((const char*)pshGram, pSize);
	ip_header2->check  = checksum_calc((const char*)segment, ip_header2->tot_len);

	*packet_out = segment;
	*pack_len   = ip_header2->tot_len;

	free(pshGram);

}


// Function To  Quickly Check The Seq Nums and Store them
uint32_t parse_packet(char* buffer, uint32_t *seq, uint32_t *ack) {
	struct iphdr *ipHeader = (struct iphdr*)buffer;
	struct tcphdr *tcpHeader = (struct tcphdr*)(buffer + sizeof(struct iphdr));
	struct sockaddr_in src;
	src.sin_addr.s_addr = ipHeader->saddr;
	printf("[*] ACK Flag: %d\n", (unsigned int)tcpHeader->ack);
	printf("[*] Ack_seq: %u\n", ntohl(tcpHeader->ack_seq));
	printf("test");
	printf("SOURCE: %s\n", inet_ntoa(src.sin_addr));

	*ack = ntohl(tcpHeader->ack_seq);
	*seq = ntohl(tcpHeader->seq);
}

int main(int argc, char** argv[])
{
	if (argc != 4) {
		printf("<source ip>, <target ip>, <port>");
		return 1;
	}

	// Get Random Seed For RAND()
	srand(time(NULL));

	// Create RAW Socket using TCP(IPPROTO_TCP Lets Us Only Worry About the layer 3 and 4 headers, not the layer 2 header)
	int sockFD = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (sockFD < 0) {
		printf("Socket Failed\tERROR: %s", strerror(errno));
		return 1;
	}

	// Declare Struct For our Target
	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[3]));
	// Convert Ip to network bytes order
	if (inet_pton(AF_INET, argv[2], &dest.sin_addr) != 1){
		printf("Failed Convetint IP");
		return 1;
	}

	struct sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(rand() % 65535);
	if(inet_pton(AF_INET, argv[1], &sAddr.sin_addr) != 1) {
		printf("Failed COnverting Host IP");
		return 1;
	}

	printf("PORT NO: %d\n", ntohs(sAddr.sin_port));


	// Tell Kernel We Will Be Setting Headers
	int one = 1;
	int *val = &one;
	if(setsockopt(sockFD, IPPROTO_IP, IP_HDRINCL ,&one, sizeof(one)) < 0) {
		printf("Failed Setting Socket options");
		return 1;
	}

	char* packet;
	int pack_len;
	syn_packet(&sAddr, &dest, &packet, &pack_len);
	int sent;

	if ((sent = sendto(sockFD, packet, pack_len, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr))) < 0) {
		printf("Failed Sending Packet");
	}
	printf("[*] Succesfully Sent Packet!\n");
	sleep(3);

	char syn_ack_buffer[PACKET_LEN];
	int recveived = recvfrom(sockFD, syn_ack_buffer, sizeof(syn_ack_buffer), 0, NULL, NULL);
	printf("recveived %d-Bytes\n", recveived);

	uint32_t seq_num, ack_seq_num;
	parse_packet(syn_ack_buffer, &seq_num, &ack_seq_num);
	printf("[*] Seq: %u\n", seq_num);
	int new_seq = seq_num + 1;

	printf("[*] New Seq: %u\n", new_seq);

	//void send_ack(struct sockaddr_in* src, struct sockaddr_in* dst, char** packet_out, int* pack_len, int32_t seq, int32_t ack)
	send_ack(&sAddr, &dest, &packet, &pack_len, new_seq, ack_seq_num);

	if((sent = sendto(sockFD, packet, pack_len, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr))) < 0) {
		printf("Failed Sending ACK\n");
		return 1;
	}

	printf("Succesfully Sent ACK Packet\n");

	

	//sleep(20);
	close(sockFD);
	return 0;
}
