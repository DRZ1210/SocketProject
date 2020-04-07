/*************************************************************************
	> File Name: client.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 16时50分21秒
 ************************************************************************/

#include "head.h"
#include "tcp_client.h"
#include "common.h"

int main(int argc, char **argv) {
    if (argc - 3) {
        fprintf(stderr, "Usage: %s ip port\n", argv[0]);
        return 1;
    }

    char msg[512] = {0};
    int sockfd;

    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        return 2;
    }

    //设置socket文件属性为非阻塞IO，文件一般默认时阻塞的
    make_nonblock_ioctl(sockfd);

    recv(sockfd, msg, sizeof(msg), 0);
    printf("recv: %s\n", msg);
    close(sockfd);

    return 0;
}
