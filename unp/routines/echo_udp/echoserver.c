#include "unp.h"

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);

static void recvfrom_int(int);

static int count;

void dg_echo_mass(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);

int main(int argc, char **argv) {
    int                 sockfd;
    int                 retval;
    struct sockaddr_in  servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    // servaddr.sin_port = htons(7); // echo port

    retval = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (retval != 0) {
        perror("bind error");
        exit(-1);
    }

    // dg_echo(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    dg_echo_mass(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
}

void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen) {
    int         n;
    socklen_t   len;
    char        msg[BUF_SIZE];

    for (; ;) {
        len = clilen;
        n = recvfrom(sockfd, msg, BUF_SIZE, 0, pcliaddr, &len);
        if (n == -1) {
            perror("recvfrom error");
            exit(-1);
        }

        if (sendto(sockfd, msg, n, 0, pcliaddr, len) == -1) {
            perror("sendto error");
            exit(-1);
        }
    }
}

void dg_echo_mass(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen) {
    int         n;
    socklen_t   len;
    char        msg[BUF_SIZE];

    signal(SIGINT, recvfrom_int);

    n = 220 * 1024;
    // set the socket receive buffer to 240KB
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

    for (; ;) {
        len = clilen;
        recvfrom(sockfd, msg, BUF_SIZE, 0, pcliaddr, &len);
        count++;
    }
}

static void recvfrom_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}
