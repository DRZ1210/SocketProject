/*************************************************************************
	> File Name: tcp_server.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 16时43分47秒
 ************************************************************************/

#include "head.h"

//参数：服务器本地端口号port
//功能：为server端创建一个处于监听状态的套接字
//返回值：返回套接字文件描述符

int socket_create(int port) {
    int server_listen;
    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    struct linger m_linger;
    m_linger.l_onoff = 1;
    m_linger.l_linger = 5;
    if (setsockopt(server_listen, SOL_SOCKET, SO_LINGER, &m_linger, (socklen_t)sizeof(m_linger)) < 0) {
        return -1;
    }
    int flag = 1;
    if (setsockopt(server_listen, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
        return -1;
    }


    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    if (listen(server_listen, 20) < 0){
        return -1;
    }
    return server_listen;
}

