// standard librarys
#include <stdio.h>
#include <string.h>    
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>


// network librarys
#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <netinet/ip.h>   
#include <netinet/ip_icmp.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>

// system librarys
#include <signal.h>
#include <unistd.h> 



#define BASE_PING_SIZE 5 // todo: change to dynamic 
#define JUNK_SIZE 56           // payload size of the icmp packet  todo: allow the user to specify the payload size
#define DEFAULT_TTL  64
#define VERSION_IPV4 4
#define PROTOCOL_ICMP 1
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0
															 

              
typedef struct pingStats {
	double *times; // stores ping times
	int seq_id;                // stores the icmp sequance ID
	int sent;                  // stores the amount of packets sent
	int recevied;
} pingStats;

typedef struct Args {
	int max_pings;
	int speed;
} Args;

struct pingStats global_stats;


/*
	calculates the average of the round trip times
*/

double calculate_average(struct pingStats *stat) {
	if(stat->seq_id <= 0) {
	    return 0.0;
	}
    int array_size = stat->seq_id;

		
	double count = 0.0;

	for (int j = 0; j < stat->seq_id; j++) {	 
		count += stat->times[j];
	}
	return count / array_size;
}


int compare(const void *a, const void *b) {
    double val1 = *(double *)a;
    double val2 = *(double *)b;
    
    if (val1 < val2) return -1;
    else if (val1 > val2) return 1;
    else return 0;
}

double calculate_median(struct pingStats *stats) {
	// sort array smallest to largest
	// if the total amount of elemants is even, find the average of the 2 middle ones and return it
	// if the total amount is odd then return the middle elemant
	
	qsort(stats->times, stats->seq_id, sizeof(double), compare);
	double median;

	
	
	if(stats->seq_id % 2 == 0) {
		double middle = stats->times[stats->seq_id / 2];
		double middle2 =  stats->times[stats->seq_id / 2 -1];
		median = (middle + middle2) / 2;
	} else {
		median =  stats->times[stats->seq_id / 2];
	}
	return median;
}


double calculate_min(struct pingStats *stats) {
	  if(stats->seq_id <= 0) {
	  	return 0;
	  }
	  double smallest = stats->times[0];
    for(int i = 0; i < stats->seq_id; i++) {
        if(smallest >  stats->times[i]) {
        	smallest =  stats->times[i];
        }

    }
    return smallest;
}


void handle_sigint(int sig) {
		if(global_stats.seq_id <= 0) {
			exit(0);
		}
		
		int dropped = 0;
		int packet_loss = global_stats.sent > 0 ? (int)((double)dropped * 100 / global_stats.sent) : 0;
		

		printf("\n     ----- ping statistics -----\n");
		printf("rtt min/avg/med = %.3f/%.3f/%.3f ms\n", calculate_min(&global_stats), calculate_average(&global_stats), calculate_median(&global_stats));
		printf("Packets sent: %d, packets received: %d %d%% packet loss", global_stats.sent, global_stats.recevied, packet_loss);
		free(global_stats.times);
		exit(0);
}


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


void parse_packet(unsigned char* recv_packet, double time, int size, char* target) {
	struct iphdr *iphdr = (struct iphdr*)recv_packet;
	struct icmphdr *icmphdr = (struct icmphdr*)(recv_packet + sizeof(struct iphdr));

	//quick check to see the recevied packet is the reply to our echo request
	if(icmphdr->type != ICMP_ECHO_REPLY) {
		return;
	}



	printf("Received %d-Bytes from %s: icmp_seq=%d ttl=%d time=%.3fms\n", size, target, ntohs(icmphdr->un.echo.sequence), iphdr->ttl, time);
}

struct icmphdr* create_packet(unsigned char *packet, int sock, char* target_ip) {
	struct iphdr *iphdr = (struct iphdr*)packet;
	struct icmphdr *icmphdr = (struct icmphdr*)(packet + sizeof(struct iphdr));
	unsigned char junk[JUNK_SIZE];
	memset(junk, 0xAA, JUNK_SIZE);
	memcpy(packet + sizeof(struct iphdr) + sizeof(struct icmphdr), junk, JUNK_SIZE);

	struct ifreq ifr;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
	ioctl(sock, SIOCGIFADDR, &ifr);

	iphdr->ihl = 5;
	iphdr->version = VERSION_IPV4;
	iphdr->tos = 0;
	iphdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + JUNK_SIZE);
	iphdr->id = htons(12312);
	iphdr->frag_off = htons(0);
	iphdr->ttl = DEFAULT_TTL;
	iphdr->protocol = PROTOCOL_ICMP;
	iphdr->check = 0;
	iphdr->saddr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
	inet_pton(AF_INET, target_ip, &(iphdr->daddr));
	



	icmphdr->type = ICMP_ECHO_REQUEST;                           
	icmphdr->code = 0;                             
	icmphdr->checksum  = 0;
	icmphdr->un.echo.id = htons(15);
	icmphdr->un.echo.sequence = htons(0);
	icmphdr->checksum = checksum_calc((const char*)icmphdr, sizeof(struct icmphdr) + JUNK_SIZE);

	return icmphdr;
}


int check_hostname_ip(char* buf, char* input) {
	struct sockaddr_in temp_addr;

	if(inet_pton(AF_INET, input, &(temp_addr.sin_addr)) == 1) {
		strncpy(buf, input, INET_ADDRSTRLEN);
		
	} else {
		
		struct addrinfo hints, *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;

		if (getaddrinfo(input, NULL, &hints, &res) != 0) {
			fprintf(stderr, "[!] Invalid IP or domain: %s\n", input);
			return 0;
		}

		
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
		inet_ntop(AF_INET, &(ipv4->sin_addr), buf, INET_ADDRSTRLEN);
		
		freeaddrinfo(res);
	}
}


void parse_arguments(struct Args *Args, int arg_count, char **arguments) {
	int c;
	int opt;
	void print_help();

	while((c = getopt(arg_count, arguments, "c:s:h")) != -1) {
		switch (c) {
		  case 'c':
		  	Args->max_pings = atoi(optarg);
		  	break;
		  case 's':
		  	Args->speed = atoi(optarg);
		  	break;
		  case 'h':
		  	print_help();
		  	break;
		}
	} 

}

void print_help() {
	printf("-h   Print Help\n");
	printf("-c   Max Echo Requests To Send | default=150\n");
	printf("-s   delay between requests | default=1s\n");
	exit(0);

}

int main(int argc, char const *argv[])
{
	struct Args Args = { -1, 1 };
	char target_ip[INET_ADDRSTRLEN];
	if(argc < 2) {
		fprintf(stderr, "Usage: ./pinger <hostname/ip>\n");
		return 0;
	}

	if(!check_hostname_ip(target_ip, argv[1])) {
		fprintf(stderr, "Failed to convert hostname(%s)\n", strerror(errno));
		return 1;
	}
	parse_arguments(&Args, argc, (char**)argv);

	signal(SIGINT, handle_sigint);

	global_stats.sent = 0;
	global_stats.recevied = 0;
	global_stats.seq_id = 0;

	
	
	
	

	int sock;
	double time;
	struct sockaddr_in sockStruct;
	unsigned char packet[sizeof(struct iphdr) + sizeof(struct icmphdr) + JUNK_SIZE];


	if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		fprintf(stderr, "Error: %s(Are you running as root?)\n", strerror(errno));
		close(sock);
		return 1;
	} 

	
	




	memset(&sockStruct, 0, sizeof(sockStruct));
	sockStruct.sin_family = AF_INET;
	sockStruct.sin_port = 0;
	inet_pton(AF_INET, target_ip, &sockStruct.sin_addr);

	int one = 1;
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
	    perror("setsockopt");
	    return 1;
	}

	

	struct icmphdr *icmp = create_packet(packet, sock, target_ip);

	

	
	unsigned char recv_buffer[sizeof(struct iphdr) + sizeof(struct icmphdr) + JUNK_SIZE];
	int i = 0;
	
	struct timespec end, start;
	
	printf("[+] Pinging %s (%s) with %d(%d) Bytes of data\n", argv[1], target_ip, JUNK_SIZE, JUNK_SIZE + sizeof(struct iphdr) + sizeof(struct icmphdr));

	size_t capacity = BASE_PING_SIZE;
	global_stats.times = calloc(capacity, sizeof(double));

	while (Args.max_pings == -1 || global_stats.sent < Args.max_pings) {
		
		
		memset(recv_buffer, 0, sizeof(recv_buffer));

		// start timer
		clock_gettime(CLOCK_MONOTONIC, &start);

		// send the icmp echo request
		if((sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&sockStruct, sizeof(sockStruct))) < 0) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			return 0;
		}
		global_stats.sent++;

		
		int recv_size = 0;

		// receive the icmp echo reply
		if((recv_size = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0 ,NULL, NULL)) < 0) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			return 0;
		}
		if(recv_size > 0) {
			global_stats.recevied++;
		}

		// end the timer
		clock_gettime(CLOCK_MONOTONIC, &end);

		// calculate the milla seconds between sending and receiving
		time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;


	    if (global_stats.seq_id >= capacity) {
	        size_t new_capacity = capacity + (capacity / 2);
	        double *temp = realloc(global_stats.times, new_capacity * sizeof(double));
	        if (!temp) {
	            fprintf(stderr, "Realloc failed\n");
	            free(global_stats.times);
	            exit(EXIT_FAILURE);
	        }
	        global_stats.times = temp;
	        capacity = new_capacity;
	    }

		// store the round trip time into the array
		global_stats.times[global_stats.seq_id] = time;

		// calculate the size of the icmp reply
		int icmp_size = recv_size - sizeof(struct iphdr);
		
		parse_packet(recv_buffer, time, icmp_size, target_ip);

		// update the sequance number
		icmp->un.echo.sequence = htons(++global_stats.seq_id);

		// need to fix the checksum since we altered the packets
		icmp->checksum = 0;
		icmp->checksum = checksum_calc((const char*)icmp, sizeof(struct icmphdr) + JUNK_SIZE);
		
		sleep(Args.speed);
	}

	

	return 0;
}
