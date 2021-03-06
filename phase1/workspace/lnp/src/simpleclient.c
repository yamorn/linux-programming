/*
 * simpleclient.c
 *
 *  Created on: May 4, 2015
 *      Author: yamorn
 */

#include "gen.h"

void process(FILE *, int);

int main(int argc, char **argv) {
	int listenfd;
	struct sockaddr_in servaddr;
	int ret;

	if (argc != 2) {
		fprintf(stderr, "usage:%s <ip address>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	ret = connect(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (ret == -1) {
		perror("connect error");
		exit(EXIT_FAILURE);
	}

	process(stdin, listenfd);

	exit(EXIT_SUCCESS);
}

void process(FILE *fp, int fd) {
	char writebuf[MAXLEN], readbuf[MAXLEN];
	int n;
	while (fgets(writebuf, MAXLEN, fp) != NULL) {
		writen(fd, writebuf, strlen(writebuf));

//		if ((n = readline(fd, readbuf, MAXLEN)) <= 0) {
//			perror("client: server terminated permaturely");
//			exit(1);
//		}
//		printf("n=%d\n", n);
//		write(fileno(stdout), readbuf, n);
//		fflush(stdout);
	}
}
