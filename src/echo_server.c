#include "unp.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
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

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t child_pid;
    char buff[MAXLINE];
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;
    struct sigaction sa;

    (void) argc;
    (void) argv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7);

    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    bzero(&sa, sizeof(sa));
    sa.sa_handler = sig_chld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

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

        child_pid = fork();
        if (child_pid < 0)
            err_sys("fork error");

        if (child_pid == 0)
        {
            close(listenfd);
            while ((n = read(connfd, recvline, MAXLINE)) > 0)
            {
                recvline[n] = 0;
                fputs(recvline, stdout);
                strcpy(buff, recvline);
                write(connfd, buff, strlen(buff));
            }

            close(connfd);
            _exit(0);
        }

        close(connfd);
    }
}
