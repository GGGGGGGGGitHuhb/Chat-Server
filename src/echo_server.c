#include "unp.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    char buff[MAXLINE];
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7);
    
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for ( ; ; )
    {
        connfd = accept(listenfd, (SA *) NULL, NULL);
        while ((n = read(connfd, recvline, MAXLINE)) > 0)
        {
            recvline[n] = 0;
            fputs(recvline, stdout);
            strcpy(buff, recvline);
            write(connfd, buff, strlen(buff));
        }

        close(connfd);
    }
}