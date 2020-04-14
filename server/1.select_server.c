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

#define CLIENTSIZE 50
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
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }
    int server_listen, fd;
    if ((server_listen = socket_create(atoi(argv[1]))) < 0) {
        perror("socket_create");
        exit(1);
    }

    int client[CLIENTSIZE];
    memset(client, -1, sizeof(client));
    make_nonblock(server_listen); 

    fd_set rfds, wfds, efds;
    int maxfd = server_listen;

    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        FD_SET(server_listen, &rfds);

        for (int i = 0; i < CLIENTSIZE; i++) {
            if (client[i] == server_listen) continue;
            if (client[i] > 0) {
                if (maxfd < client[i]) maxfd = client[i];
                FD_SET(client[i], &rfds);
            }
        }

        //若timeout结构体指针为NULL，此时无限等待
        if (select(maxfd + 1, &rfds, NULL, NULL, NULL) < 0) {
            perror("select");
            return 1;
        }

        if (FD_ISSET(server_listen, &rfds)) {
            printf("connect ready on serverlisten\n");
            if ((fd = accept(server_listen, NULL, NULL)) < 0) {
                perror("acccept");
                return 1;
            }
            if (fd > CLIENTSIZE) {
                printf("too many clients\n");
                close(fd);
            } else {
                make_nonblock(fd);
                if (client[fd] == -1) {
                    client[fd] = fd;
                }
            }
        }
        
        for (int i = 0; i < CLIENTSIZE; i++) {
            if (i == server_listen) continue;
            if (FD_ISSET(i, &rfds)) {
                char buff[BUFFSIZE] = {0};
                int retval = recv(i, buff, BUFFSIZE, 0);
                if (retval <= 0) {
                    printf("Logout\n");
                    client[i] = -1;
                    close(fd);
                    continue;
                }
                printf("<recv>: %s\n", buff);
                chstr(buff);
                send(fd, buff, sizeof(buff), 0);
            }
        }




    }


    return 0;
}
