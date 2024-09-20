#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <net/ethernet.h>
#include <signal.h>
#include <netpacket/packet.h>
#include <linux/tcp.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <netinet/ip_icmp.h>



#define PACKET_SIZE 43
#define ARP_PROTOCOL 0x0806 
#define IPv4_PROTOCOL 0x0800
#define HARDWARE_LENGTH 6
#define PROTOCOL_LENGTH 4

#define info(x, ...) printf("[+] " x "\n", ##__VA_ARGS__)
#define okay(x, ...) printf("[*] " x "\n", ##__VA_ARGS__)
#define error(x, ...) printf("[-] " x "\n", ##__VA_ARGS__)

#define COLOR_1 "\033[38;5;20m"
#define COLOR_2 "\033[38;5;46m"
#define COLOR_3 "\033[38;5;27m"
#define dash_color "\033[38;5;44m"

// Text Colors
#define TEXT_BLACK      "\033[30m"
#define TEXT_RED        "\033[31m"
#define TEXT_GREEN      "\033[32m"
#define TEXT_YELLOW     "\033[33m"
#define TEXT_BLUE       "\033[34m"
#define TEXT_MAGENTA    "\033[35m"
#define TEXT_CYAN       "\033[36m"
#define TEXT_WHITE      "\033[37m"

#define RESET              "\033[0m"
#define BOLD        "\033[1m"

// Bright Text Colors
#define TEXT_BRIGHT_BLACK  "\033[90m"
#define TEXT_BRIGHT_RED    "\033[91m"
#define TEXT_BRIGHT_GREEN  "\033[92m"
#define TEXT_BRIGHT_YELLOW "\033[93m"
#define TEXT_BRIGHT_BLUE   "\033[94m"
#define TEXT_BRIGHT_MAGENTA "\033[95m"
#define TEXT_BRIGHT_CYAN   "\033[96m"
#define TEXT_BRIGHT_WHITE  "\033[97m"

// for the promtp 


#define one  "\033[38;5;18m"
#define two  "\033[38;5;19m"
#define three  "\033[38;5;20m"
#define foor  "\033[38;5;21m"
#define five  "\033[38;5;26m"
#define six  "\033[38;5;27m"
#define seven  "\033[38;52;28m"
#define eigth  "\033[38;5;33m"
#define nine  "\033[38;5;39m"
#define ten  "\033[38;5;44m"
#define eleven  "\033[38;5;45m"
#define twelve  "\033[38;5;46m"
#define thirteen  "\033[38;5;47m"



// Struct for ARP Reply,Read Below For what each Entry mEans
struct arphdr {
    uint16_t hw_type;         
    uint16_t proto_type;      
    uint8_t hw_len;            
    uint8_t proto_len;        
    uint16_t op;               
    uint8_t sender_hw_addr[HARDWARE_LENGTH]; 
    uint8_t sender_proto_addr[PROTOCOL_LENGTH]; 
    uint8_t target_hw_addr[HARDWARE_LENGTH]; 
    uint8_t target_proto_addr[PROTOCOL_LENGTH]; 
};

int scan_network(char* ip_range);
void parse_packet(unsigned char *packet);
char* get_ieee(uint8_t *mac);
int entry(char* ip, char* gateway, char* tMac, char* rMac);
int main(void);
void print_colors(char* str, int okay_or_error);
int sniffer_entry(void);
void print_input();
