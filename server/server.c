/*************************************************************************
	> File Name: server.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 17时46分39秒
 ************************************************************************/

#include "head.h"
#include "tcp_server.h"

#define MAX_CLIENT 512

struct Client { //客户在线、对应线程号、文件描述符
    int flag, fd;
    pthread_t tid;
};

struct Client *client;

int find_pos() { //找到下标最小的空tid
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].flag == 0) {
            return i;
        }
    }
    return -1;
}

void change_str(char *str) { //将客户发送的信息，转成大写，返回给客户
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 32;
        }
    }
}

void *work(void *arg) {
    int *sub = (int *)arg;
    int fd = client[*sub].fd;
    if (send(fd, "You Are Here\n", sizeof("You Are Here\n"), 0) < 0) {
        perror("send");
        close(fd);
        client[*sub].flag = 0;
        return NULL;
    }
    while (1) {
        char msg[512] = {0};
        if (recv(fd, msg, sizeof(msg), 0) <= 0) { //出错或对方关闭连接
            break;
        }
        change_str(msg);
        send(fd, msg, strlen(msg), 0);
    }
    close(fd);
    client[*sub].flag = 0;
    return NULL;
}


int main(int argc, char **argv) {
    int port, server_listen;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    
    //调用在..common/tcp_server.c中写好的函数，创建处于监听状态的套接字
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 2;
    }

    pthread_t tid;
    
    client = (struct Client *)calloc(MAX_CLIENT, sizeof(struct Client));

    while (1) {
        int sub, fd;
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
        }
        
        printf("New Client Login\n");

        if ((sub = find_pos()) < 0) {
            fprintf(stderr, "Array Full\n");
            close(fd);
            continue;
        }
        client[sub].flag = 1;
        client[sub].fd = fd;

        //创建子线程，处理每一个用户请求
        pthread_create(&client[sub].tid, NULL, work, (void *)&sub);
    }

    return 0;
}

