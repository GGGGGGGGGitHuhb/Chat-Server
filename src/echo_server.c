#include "unp.h"
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int i;
    int listenfd;
    int connfd;
    int sockfd;
    int maxfd;
    int maxi;
    int nready;
    ssize_t n;
    int client[FD_SETSIZE];
    fd_set allset;
    fd_set rset;
    char buf[MAXLINE];
    struct sockaddr_in servaddr;

    (void) argc;
    (void) argv;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; ++i)
    {
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (;;)
    {
        rset = allset;
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset))
        {
            connfd = Accept(listenfd, (SA *) NULL, NULL);

            for (i = 0; i < FD_SETSIZE; ++i)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if (i == FD_SETSIZE)
            {
                Close(connfd);
                err_quit("too many clients");
            }

            FD_SET(connfd, &allset);
            if (connfd > maxfd)
            {
                maxfd = connfd;
            }
            if (i > maxi)
            {
                maxi = i;
            }

            if (--nready <= 0)
            {
                continue;
            }
        }

        for (i = 0; i <= maxi; ++i)
        {
            if ((sockfd = client[i]) < 0)
            {
                continue;
            }

            if (FD_ISSET(sockfd, &rset))
            {
                n = read(sockfd, buf, MAXLINE);
                if (n < 0)
                {
                    err_sys("read error");
                }
                else if (n == 0)
                {
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                else
                {
                    Writen(sockfd, buf, (size_t) n);
                }

                if (--nready <= 0)
                {
                    break;
                }
            }
        }
    }
}
