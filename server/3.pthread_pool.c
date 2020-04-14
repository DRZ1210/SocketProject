/*************************************************************************
	> File Name: server.c
	> Author: DRZ 
	> Mail: 1927157164@qq.com
	> Created Time: 2020年04月07日 星期二 18时22分43秒
 ************************************************************************/

#include "../common/head.h"
#include "../common/tcp_server.h"
#include "../common/common.h"
#include "../common/color.h"

#define MAXTASK 100
#define MAXTHREAD  20

typedef struct {
    int sum;
    int *fd;
    int head, tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

//小写转大写
char ch_char(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}

void do_echo(int fd) {
    char buff[512] = {0}, ch;
    int ind = 0;
    while (1) {
        if (recv(fd, &ch, 1, 0) <= 0) {
            break;
        }
        if (ind < sizeof(buff)) {
            buff[ind++] = ch_char(ch);
        }
        if (ch == '\n') {
            send(fd, buff, ind, 0);
            ind = 0;
            continue;
        }
    }
}

//任务队列初始化
void TaskQueueInit(TaskQueue *queue, int sum) {
    queue->sum = sum;
    queue->fd = (int *)calloc(sum, sizeof(int));
    queue->head = queue->tail = 0;
    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->cond), NULL);
}

//队列push操作
void TaskQueuePush(TaskQueue *queue, int fd) {
    pthread_mutex_lock(&(queue->mutex));
    queue->fd[queue->tail++] = fd;
    printf(L_YELLOW"<TaskPush>: "NONE"%d\n", fd);
    if (queue->tail == queue->sum) {
        printf(L_RED"<Queue End>: "NONE"%d\n", fd);
        queue->tail = 0;
    }
    pthread_mutex_unlock(&(queue->mutex));
    pthread_cond_signal(&(queue->cond));
}

//队列pop操作
int TaskQueuePop(TaskQueue *queue) {
    pthread_mutex_lock(&(queue->mutex));
    while (queue->tail == queue->head) {
        pthread_cond_wait(&(queue->cond), &(queue->mutex));
    }
    int fd = queue->fd[queue->head++];
    printf(L_GREEN"<TaskPop>: "NONE"%d\n", fd);
    if (queue->head == queue->sum) {
        printf(L_RED"<Queue End>: "NONE"%d\n", fd);
        queue->head = 0;
    }
    pthread_mutex_unlock(&(queue->mutex));
}

void *thread_run(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid); //将线程分离,线程不死亡，有事干，没事睡

    TaskQueue *queue = (TaskQueue *)arg;
    while (1) {
        int fd = TaskQueuePop(queue);
        do_echo(fd);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    int port, server_listen, fd;
    port = atoi(argv[1]);
    
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }

    TaskQueue queue;
    TaskQueueInit(&queue, MAXTHREAD);
    pthread_t *tid = (pthread_t *)calloc(MAXTHREAD, sizeof(pthread_t));

    for (int i = 0; i < MAXTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)&queue);
    }


    while (1) {
        int sub, fd;
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            exit(1);
        }
        printf(L_BLUE"<Login>: "NONE"%d\n", fd);
        TaskQueuePush(&queue, fd);
    }

    return 0;
}

