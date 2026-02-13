#include "unp.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void sig_chld(int signo)
{
    (void) signo; // 该参数故意未使用
    while (waitpid(-1, NULL, WNOHANG) > 0) // 循环回收多个僵尸进程
    {}
}

static void str_echo_func(int sockfd)
{
    ssize_t n;
    char recvline[MAXLINE];

    while ((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        Writen(sockfd, recvline, (size_t)n);
        recvline[n] = 0;
        fputs(recvline, stdout);
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

    (void) argc; // 该参数故意未使用
    (void) argv; // 该参数故意未使用

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 服务器初始化流程
    bzero(&servaddr, sizeof(servaddr)); // 结构体清零，避免脏值
    servaddr.sin_family = AF_INET; // 填充结构体字段
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7);
    
    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    bzero(&sa, sizeof(sa));
    sa.sa_handler = sig_chld; // sa_handler 是函数指针
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    for ( ; ; )
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

        if((child_pid = Fork()) == 0)
        {
            Close(listenfd);
            str_echo_func(connfd);
            Close(connfd);
            _exit(0);
        }
        
        Close(connfd);

    }
}