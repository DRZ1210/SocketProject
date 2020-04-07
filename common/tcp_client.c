/*************************************************************************
	> File Name: tcp_client.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 16时39分17秒
 ************************************************************************/

#include "head.h"

//参数：服务端ip地址和port端口号
//功能：为client端创建一个套接字连接
//返回值：返回套接字文件描述符

int socket_connect(char *host, int port) {
    int sockfd;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    printf("Socket create.\n");
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return -1;
    }
    return sockfd;
}

