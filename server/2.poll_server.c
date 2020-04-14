/*************************************************************************
	> File Name: tmp_server.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月11日 星期六 18时12分42秒
 ************************************************************************/

#include "../common/color.h"
#include "../common/common.h"
#include "../common/tcp_server.h"
#include "../common/head.h"

#define POLLSIZE 100
#define BUFFSIZE 512

void chstr(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 32;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port", argv[0]);
        exit(1);
    }
    int server_listen, fd;
    if ((server_listen = socket_create(atoi(argv[1]))) < 0) {
        perror("socket_create");
        exit(1);
    }

    struct pollfd event_set[POLLSIZE];
    for (int i = 0; i < POLLSIZE; i++) {
        event_set[i].fd = -1;
    }
    event_set[0].fd = server_listen;
    event_set[0].events = POLLIN; //关注输入

    while (1) {
        int retval;
        if ((retval = poll(event_set, POLLSIZE, -1)) < 0) {
            perror("poll");
            return 1;
        }
        //event_set[0]对应的POLLIN事件发生了
        if (event_set[0].revents & POLLIN) {
            if ((fd = accept(server_listen, NULL, NULL)) < 0) {
                perror("accept");
                continue;
            }
            retval--;
            int i;
            for (i = 1; i < POLLSIZE; i++) {
                if (event_set[i].fd < 0) { 
                    //找到最小的没有使用的下标
                    event_set[i].fd = fd;
                    event_set[i].events = POLLIN;
                    break;
                }
            }
            if (i == POLLSIZE) {
                printf("Too Many Client\n");
            }

        }
        for (int i = 1; i < POLLSIZE; i++) {
            if (event_set[i].fd < 0) continue;

            if (event_set[i].revents & (POLLIN | POLL_HUP | POLL_ERR)) {
                retval--;
                char buff[BUFFSIZE] = {0};
                if (recv(event_set[i].fd, buff, BUFFSIZE, 0) > 0) {
                    printf("Recv: %s\n", buff);
                    chstr(buff);
                    /*
                    for (int i = 0; i < strlen(buff); i++) {
                        buff[i] = ch_char(buff[i]);
                    }*/
                    send(event_set[i].fd, buff, strlen(buff), 0);
                } else {
                    close(event_set[i].fd);
                    event_set[i].fd = -1;
                }
            }
            if (retval <= 0) break;
        }
    }



    return 0;
}
