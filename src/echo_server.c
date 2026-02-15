#include "unp.h"
#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static void sig_chld(int signo)
{
    (void) signo;
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
}

static void str_echo(int sockfd)
{
    ssize_t n;
    char recvline[MAXLINE];

    while ((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        Writen(sockfd, recvline, (size_t) n);
    }

    if (n < 0)
    {
        err_sys("str_echo: read error");
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t child_pid;
    struct sockaddr_in servaddr;
    struct sigaction sa;

    (void) argc;
    (void) argv;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    bzero(&sa, sizeof(sa));
    sa.sa_handler = sig_chld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    Sigaction(SIGCHLD, &sa, NULL);

    for (;;)
    {
        connfd = accept(listenfd, (SA *) NULL, NULL);
        if (connfd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            err_sys("accept error");
        }

        child_pid = Fork();
        if (child_pid == 0)
        {
            Close(listenfd);
            str_echo(connfd);
            Close(connfd);
            _exit(0);
        }

        Close(connfd);
    }
}
