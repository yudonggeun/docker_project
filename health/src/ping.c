/*
 * modify by yudoneeun 2017154022
 *
 * copy from https://github.com/hayderimran7/advanced-socket-programming/blob/master/ping.c
 * 
 * 
 */
#include "../include/health.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <time.h>

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME 	2  
#define MAX_NO_PACKETS  2 

/* function */

unsigned short cal_chksum(unsigned short *addr, int len);

int pack(int pack_no, char* sendpacket);

int send_packet(int sockfd, struct sockaddr_in* dest_addr);

int recv_packet(int sockfd, int nsend);

int unpack(char* buf, int len, struct timeval* tvrecv);

void tv_sub(struct timeval *out, struct timeval *in);

unsigned short cal_chksum(unsigned short *addr, int len)

{

	int nleft = len;

	int sum = 0;

	unsigned short *w = addr;

	unsigned short answer = 0;

	while (nleft > 1)

	{
		sum +=  *w++;

		nleft -= 2;

	}



	if (nleft == 1)

	{

		*(unsigned char*)(&answer) = *(unsigned char*)w;

		sum += answer;

	}

	sum = (sum >> 16) + (sum &0xffff);

	sum += (sum >> 16);

	answer = ~sum;

	return answer;

}

int pack(int pack_no, char* sendpacket)

{

	int i, packsize;

	struct icmp *icmp;

	struct timeval *tval;

	icmp = (struct icmp*)sendpacket;

	icmp->icmp_type = ICMP_ECHO;

	icmp->icmp_code = 0;

	icmp->icmp_cksum = 0;

	icmp->icmp_seq = pack_no;

	icmp->icmp_id = getpid();

	packsize = 8+56;

	tval = (struct timeval*)icmp->icmp_data;

	gettimeofday(tval, NULL); 

	icmp->icmp_cksum = cal_chksum((unsigned short*)icmp, packsize); 

	return packsize;

}

int send_packet(int sockfd, struct sockaddr_in* dest_addr)

{

	int packetsize;

	int nsend=0;

	char sendpacket[PACKET_SIZE];

	while (nsend < MAX_NO_PACKETS)

	{
		nsend++;

		packetsize = pack(nsend, sendpacket); 

		if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*)

					dest_addr, sizeof(struct sockaddr_in)) < 0)

		{

			perror("sendto error");

			continue;

		} sleep(1); 

	}
	return nsend;
}

int recv_packet(int sockfd, int nsend)

{
	int nreceived = 0;

	int n, fromlen;

	extern int errno;

	char recvpacket[PACKET_SIZE];

	struct sockaddr_in from;

	struct timeval tvrecv;

	fromlen = sizeof(from);

	while (nreceived < nsend)

	{

		if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0/*MSG_DONTWAIT*/, (struct

							sockaddr*) &from, &fromlen)) < 0)

		{

			if (errno == EINTR)

				break;

			perror("recvfrom error");

			break;

		} gettimeofday(&tvrecv, NULL); 

		if (unpack(recvpacket, n, &tvrecv) ==  - 1)

			break;
		
		nreceived++;

	}

	return nreceived;
}

int unpack(char *buf, int len, struct timeval* tvrecv)

{

	int i, iphdrlen;

	struct ip *ip;

	struct icmp *icmp;

	struct timeval *tvsend;

	double rtt;

	ip = (struct ip*)buf;

	iphdrlen = ip->ip_hl << 2; 

	icmp = (struct icmp*)(buf + iphdrlen);

	len -= iphdrlen; 

	if (len < 8)



	{

		printf("ICMP packets\'s length is less than 8\n");

		return  - 1;

	} 



	if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == getpid()))

	{

		tvsend = (struct timeval*)icmp->icmp_data;

		tv_sub(tvrecv, tvsend); 

		rtt = tvrecv->tv_sec * 1000+tvrecv->tv_usec / 1000;

		//printf("%d byte : icmp_seq=%u ttl=%d rtt=%.3f ms\n", len, icmp->icmp_seq, ip->ip_ttl, rtt);

	}

	else

		return  - 1;

}

void ping(void* info)

{
	int nsend, nreceived;
	Container* con = (Container*) info;

	int sockfd;

	struct sockaddr_in dest_addr;

	struct hostent *host;

	struct protoent *protocol;

	unsigned long inaddr = 0l;

	int waittime = MAX_WAIT_TIME;

	int size = 50 * 1024;

	if ((protocol = getprotobyname("icmp")) == NULL)

	{

		perror("getprotobyname");
		restartContainer(con);
		pthread_exit(NULL);

	}

	if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)

	{

		perror("socket error");
		restartContainer(con);
		pthread_exit(NULL);

	}

	setuid(getuid());

	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

	struct timeval optVal = {MAX_WAIT_TIME, 0};
	int optLen = sizeof(optVal);

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen);

	bzero(&dest_addr, sizeof(dest_addr));

	dest_addr.sin_family = AF_INET;

	if (inaddr = inet_addr(con->addr) == INADDR_NONE)

	{

		if ((host = gethostbyname(con->addr)) == NULL)



		{

			perror("gethostbyname error");

			restartContainer(con);
			pthread_exit(NULL);

		}

		memcpy((char*) &dest_addr.sin_addr, host->h_addr, host->h_length);

	}

	else

		dest_addr.sin_addr.s_addr = inet_addr(con->addr);

	printf("PING %s(%s): %d bytes data in ICMP packets.\n", con->name, inet_ntoa

			(dest_addr.sin_addr), 56);

	nsend = send_packet(sockfd, &dest_addr);

	nreceived = recv_packet(sockfd, nsend);

	close(sockfd);

	if (nsend != nreceived) {
		fprintf(stdout, "compaer %ds : %dr\n", nsend, nreceived);
		restartContainer(con);
	}

	return ;

}

void tv_sub(struct timeval *out, struct timeval *in)

{

	if ((out->tv_usec -= in->tv_usec) < 0)

	{

		--out->tv_sec;

		out->tv_usec += 1000000;

	} out->tv_sec -= in->tv_sec;

}
