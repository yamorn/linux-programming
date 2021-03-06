#include <stdio.h>
#include <stdlib.h>
#include <string.h>				// strcpy(), memset(), memcpy()
#include <unistd.h>				// close()
#include <stdint.h>				// uint8_t, uint16_6
#include <netinet/if_ether.h>	// ETH_P_ARP, ARPOP_REQUEST, ETH_ALEN, ETH_HLEN
#include <netinet/in.h>			// IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>			// IP_MAXPACKET (which is 65535)
#include <sys/ioctl.h>			// ioctl
#include <sys/socket.h>			// socket()
#include <net/ethernet.h>		// struct ether_header,ETHERTYPE_IP,ETHERTYPE_ARP
#include <net/if.h>				// struct ifreq, IFNAMSIZ, if_nametoindex()
#include <net/if_arp.h>			// ARPHRD_ETHER
#include <netpacket/packet.h>	// struct sockaddr_ll
#include <arpa/inet.h>			// inet_pton
#include <errno.h>				// error, perror()


void getmacaddr(char *, uint8_t *);

void printmacaddr(uint8_t *);


int main(int argc, char **argv) {

	char *ifname = "wlp5s0";

	char src_ip[INET_ADDRSTRLEN];

	char dst_ip[INET_ADDRSTRLEN];

	struct ether_arp arp;

	struct ether_header etherhdr;

	struct sockaddr_ll device;

	struct in_addr src_sa, dst_sa;

	int sockfd, frame_len;

	uint8_t src_mac[6], dst_mac[6];

	uint8_t *ether_frame;



	// Set source IP
	strcpy(src_ip, "192.168.3.8");
	inet_pton(AF_INET, src_ip, &src_sa);

	// Set target IP
	strcpy(dst_ip, "192.168.3.4");
	inet_pton(AF_INET, dst_ip, &dst_sa);

	// Set source mac address
	getmacaddr(ifname, src_mac);

	// Set destination mac address
	memset(dst_mac, 0xff, sizeof(dst_mac));

	// Set ARP Packet
	arp.arp_hrd = htons(ARPHRD_ETHER);
	arp.arp_pro = htons(ETHERTYPE_IP);
	arp.arp_hln = 6;
	arp.arp_pln = 4;
	arp.arp_op = htons(ARPOP_REQUEST);
	memcpy(arp.arp_sha, src_mac, ETH_ALEN * sizeof(uint8_t));
	memcpy(arp.arp_tha, dst_mac, ETH_ALEN * sizeof(uint8_t));
	memcpy(arp.arp_spa, &src_sa, 4 * sizeof(uint8_t));
	memcpy(arp.arp_tpa, &dst_sa, 4 * sizeof(uint8_t));

	printf("arp packet length %ld\n", sizeof(arp));

	// Set Ethernet header
	memcpy(etherhdr.ether_shost, src_mac, ETH_ALEN * sizeof(uint8_t));
	memcpy(etherhdr.ether_dhost, dst_mac, ETH_ALEN * sizeof(uint8_t));
	etherhdr.ether_type = htons(ETHERTYPE_ARP);

	frame_len = ETH_HLEN + sizeof(arp);
	printf("frame_len = %d\n", frame_len);
	ether_frame = (uint8_t *)malloc(sizeof(uint8_t) * (frame_len));
	printf("ether_frame length=%ld\n", sizeof(ether_frame));
	// Package
	memcpy(ether_frame, &etherhdr, ETH_HLEN);
	memcpy(ether_frame + ETH_HLEN, &arp, sizeof(arp));

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd < 0) {
		perror("socket error.");
		exit(EXIT_FAILURE);
	}

	memset (&device, 0, sizeof (device));
  	device.sll_family = AF_PACKET;
  	if ((device.sll_ifindex = if_nametoindex (ifname)) == 0) {
    	perror ("if_nametoindex() failed to obtain interface index ");
    	exit (EXIT_FAILURE);
  	}
  	memcpy (device.sll_addr, src_mac, ETH_ALEN * sizeof (uint8_t));
  	device.sll_halen = ETH_ALEN;
  	printf("ether_frame length = %ld\n", sizeof(ether_frame));

  	if (sendto (sockfd, ether_frame, frame_len, 0, (struct sockaddr *) &device, sizeof(device)) <= 0) {
    	perror ("sendto() failed");
    	exit (EXIT_FAILURE);
  	}
  	free(ether_frame);
  	close(sockfd);
  	exit(EXIT_SUCCESS);
}

/**
 * Get MAC address
 * @param  iface interface
 * @param  macaddr
 */
void getmacaddr(char *iface, uint8_t *macaddr) {
	int fd;
	struct ifreq ifr;
	uint8_t *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);

	mac = (uint8_t *) ifr.ifr_hwaddr.sa_data;

	memcpy(macaddr, mac, sizeof(mac));
}


void printmacaddr(uint8_t *mac) {
	printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}


