#ifndef _ECHO_H_
#define _ECHO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define SERV_PORT 8080
#define BUF_SIZE  2048 * 10

ssize_t readn(int fd, void *buf, size_t nbytes);

ssize_t writen(int fd, const void *buf, size_t nbytes);

ssize_t readline(int fd, void *buf, size_t maxlen);



#endif
