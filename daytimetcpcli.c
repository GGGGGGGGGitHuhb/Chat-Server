#include "unp.h"
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char **argv) // 等价于 char *argv[]
{
    int sockfd, n; // 定义套接字描述符和 n，sockfd 本质是进程文件描述符表中的一个索引
    char recvline[MAXLINE + 1]; // 接收缓冲区最大长度。+1是为了存储字符串结束符'\0'
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: a.out <IPaddress>");

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    // AF_INET: IPv4  SOCK_STREAM: 字节流套接字  0: 给定domain与type组合的默认协议值，这里是TCP|SCTP
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr)); // 把servaddr所有字节设为0，即初始化结构体
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13); // daytime server
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) // 成功返回1，输入不是有效的网络地址返回0，系统调用失败返回-1
        // cp 是字符指针
        err_quit("inet_pton error for %s", argv[1]);

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");

    while ((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0; // null terminate，0 '\0' (char)0 都表示 ASCII 的空字符
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    if (n < 0)
        err_sys("read error");

    exit(0);
}