/*************************************************************************
	> File Name: server.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 23时19分41秒
 ************************************************************************/
#include "../common/common.h"
#include "../common/tcp_server.h"
#include "../common/chatroom.h"
#include "../common/color.h"

struct User{
    char name[20];
    int online;
    pthread_t tid;
    int fd;
};


char *conf = "./server.conf";

struct User *client;
int sum = 0;


void get_online(char *message) {
    int cnt = 0;
    char tmp[25] = {0};
    sprintf(message, "当前有 ");
    for (int i = 0; i < MAX_CLIENT; i++) {
        if(client[i].online) {
            if (cnt != 0 ) sprintf(tmp, ",%s", client[i].name);
            else sprintf(tmp, "%s", client[i].name);
            strcat(message, tmp);
            cnt++;
            if (cnt >= 50) break;
        } 
    }
    sprintf(tmp, " 等%d个用户在线", sum);
    strcat(message, tmp);
}

void send_all(struct Msg msg) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online)
            chat_send(msg, client[i].fd);
    }
}
void send_all_ex(struct Msg msg, int sub) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (sub == i) continue;
        if (client[i].online)
            chat_send(msg, client[i].fd);
    }
}

int check_name(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(client[i].name, name)) 
            return i;
    }
    return -1;
}

void *work(void *arg){
    int sub = *(int *)arg;
    int client_fd = client[sub].fd;
    struct RecvMsg rmsg;
    printf(GREEN"Login "NONE" : %s\n", client[sub].name);
    rmsg.msg.flag = 2;
    sprintf(rmsg.msg.message, "你的好友 %s 上线了，和他打个招呼吧!!!", client[sub].name);
    send_all_ex(rmsg.msg, sub);
    while (1) {
        rmsg = chat_recv(client_fd);
        if (rmsg.retval < 0) {
            printf(PINK"Logout: "NONE" %s \n", client[sub].name);
            sprintf(rmsg.msg.message, "好友 %s 已下线.", client[sub].name);
            close(client_fd);
            client[sub].online = 0;
            sum--;
            rmsg.msg.flag = 2;
            send_all(rmsg.msg);
            return NULL;
        }

        if (rmsg.msg.flag == 0) {
            printf(BLUE"%s"NONE" : %s\n",rmsg.msg.from, rmsg.msg.message);
            if (!strlen(rmsg.msg.message)) continue;
            send_all(rmsg.msg);
        } else if (rmsg.msg.flag == 1) {
            if (rmsg.msg.message[0] == '@') {
                char to[20] = {0};
                int i = 1;
                for (; i <= 20; i++) {
                    if (rmsg.msg.message[i] == ' ')
                        break;
                }
                strncpy(to, rmsg.msg.message + 1, i - 1);
                int ind;
                if ((ind = check_name(to)) < 0) {
                    sprintf(rmsg.msg.message, "%s is not online.", to);
                    rmsg.msg.flag = 2;
                    chat_send(rmsg.msg, client_fd);
                    continue;
                } else if (!strlen(rmsg.msg.message + i)) {
                    sprintf(rmsg.msg.message, "私聊消息不能为空");
                    rmsg.msg.flag = 2;
                    chat_send(rmsg.msg, client_fd);
                    continue;
                }
                printf(L_PINK"Note"NONE": %s 给 %s 发送了一条私密信息\n", rmsg.msg.from, to);
                chat_send(rmsg.msg, client[ind].fd);
            }
        } else if (rmsg.msg.flag == 4 && rmsg.msg.message[0] == '#') {
            printf(L_PINK"Note"NONE": %s查询了在线人数\n", rmsg.msg.from);
            get_online(rmsg.msg.message);
            rmsg.msg.flag = 2;
            chat_send(rmsg.msg, client_fd); 
        }
    }
    return NULL;
}


int find_sub() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!client[i].online) return i;
    }
    return -1;
}

bool check_online(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(name, client[i].name)) {
            printf(YELLOW"W"NONE": %s is online\n", name);
            return true;
        }
    }
    return false;
}


int main() {
    int port, server_listen, fd;
    struct RecvMsg recvmsg;
    struct Msg msg;
    port = atoi(get_value(conf, "SERVER_PORT"));
    client = (struct User *)calloc(MAX_CLIENT, sizeof(struct User));

    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 1;
    }
    while (1) { 
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        
        recvmsg = chat_recv(fd);
        if (recvmsg.retval < 0) {
            close(fd);
            continue;
        }
        if (check_online(recvmsg.msg.from)) {
            msg.flag = 3;
            strcpy(msg.message, "You have Already Login System!");
            chat_send(msg, fd);
            close(fd);
            continue;
            //拒绝连接：
        } 
        msg.flag = 2;
        strcpy(msg.message, "Welcome to this chat room!");
        chat_send(msg, fd);

        int sub, ret;
        sum++;
        sub = find_sub();
        client[sub].online = 1;
        client[sub].fd =fd;
        strcpy(client[sub].name, recvmsg.msg.from);
        ret = pthread_create(&client[sub].tid, NULL, work, (void *)&sub);
        if (ret != 0) {
            fprintf(stderr, "pthread_create");
        }
    }
    return 0;
}

