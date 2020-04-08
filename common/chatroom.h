/*************************************************************************
	> File Name: chatroom.h
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 23时21分43秒
 ************************************************************************/

#ifndef _CHATROOM_H
#define _CHATROOM_H

#include "head.h"

struct Msg {
    char from[20];
    int flag;
    char message[512];
};



struct RecvMsg {
    struct Msg msg;
    int retval;
};

//按理说.h头文件中只可以有函数的声明，不可以有函数的具体定义
//但是由于client和server都需要使用这里定义的函数，为了减少操作，我们直接在这里是定义函数

int chat_send(struct Msg msg, int fd) {
    if (send(fd, (void *)&msg, sizeof(msg), 0) <= 0) {
        return -1;
    }
    return 0;
}

struct RecvMsg chat_recv(int fd) {
    struct RecvMsg tmp;
    memset(&tmp, 0, sizeof(tmp));
    if (recv(fd, &tmp.msg, sizeof(struct Msg), 0) <= 0) {
        tmp.retval = -1;
    }
    return tmp;
}

#define MAX_CLIENT 512

#endif
