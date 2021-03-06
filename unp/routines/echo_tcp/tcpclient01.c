#include "unp.h"

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv){
    int                 i, sockfd[5];
    struct sockaddr_in  servaddr;

    if(argc != 2){
        printf("usage: tcpcli <IPAddress>\n");
        exit(0);
    }
    for (i = 0; i < 5; i++){
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

        if (connect(sockfd[i], (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0){
            printf("the %d socket connection failed.", i);
            exit(-1);
        }
    }

    str_cli(stdin, sockfd[0]);  /* do it all */
    exit(0);
}


void str_cli(FILE *fp, int sockfd){
    char sendline[BUF_SIZE], recvline[BUF_SIZE];

    while(fgets(sendline, BUF_SIZE, fp) != NULL){
        writen(sockfd, sendline, strlen(sendline));

        if(readline(sockfd, recvline, BUF_SIZE) == 0){
            perror("str_cli: server terminated prematurely");
            exit(-1);
        }

        fputs(recvline, stdout);
    }
}


