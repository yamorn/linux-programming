/*
 * selectserver.c
 *
 *  Created on: May 4, 2015
 *      Author: yamorn
 */
#include "gen.h"

#define BUF_SIZ 1024
/**
 * step: socket --> bind --> listen --> accept
 */

int main(int argc, char **argv) {
	int listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	fd_set rset; /* read file descriptor set */
	int fd_a[LISTENQ]; /* the queue for completely established sockets waiting to be accepted */
	int ret, maxsockfd;
	int conn_num = 0; /* current connection number */
	int i;
	char buf[BUF_SIZ];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind error");
		exit(EXIT_FAILURE);
	}

	if (listen(listenfd, LISTENQ) < 0) {
		perror("listen error");
		exit(EXIT_FAILURE);
	}

	maxsockfd = listenfd;
	memset(fd_a, 0, sizeof(fd_a));
	FD_ZERO(&rset);

	for (;;) {
		FD_SET(listenfd, &rset);
		ret = select(maxsockfd + 1, &rset, NULL, NULL, NULL);
		if (ret == -1) {
			perror("select error");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(listenfd, &rset)) { /* new connection is ready */
			socklen_t clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
			if (connfd == -1) {
				perror("accept error");
				continue;
			}

			/* add to queue */
			if (conn_num < LISTENQ) {
				fd_a[conn_num++] = connfd;
				FD_SET(connfd, &rset);
				printf("new connection client[%d] %s:%d\n", conn_num,
						inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
				if (connfd > maxsockfd)
					maxsockfd = connfd;
			} else {
				printf("max connections arrived, server exit\n");
				send(connfd, "bye", 4, 0);
				close(connfd);
				break;
			}
		}

		for (i = 0; i < conn_num; i++) {
			if (FD_ISSET(fd_a[i], &rset)) {
				/* process request */
				ret = recv(fd_a[i], buf, sizeof(buf), 0);
				if (ret <= 0) {
					printf("client[%d] close\n", i);
					close(fd_a[i]);
					FD_CLR(fd_a[i], &rset);
					fd_a[i] = 0;
				} else {
					if (ret < BUF_SIZ)
						memset(&buf[ret], '\0', 1);
					printf("client[%d] send: %s", i, buf);

				}
			}
		}
	}

	/* clean */
	for (i = 0; i < LISTENQ; i++) {
		if (fd_a[i] != 0)
			close(fd_a[i]);
	}

	exit(EXIT_SUCCESS);
}
