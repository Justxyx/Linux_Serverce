//
// Created by xm on 2022/5/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// 消息结构
struct msg {
    struct msg *next;
    int data;       // 消息数据
};

struct msg *queue;  // 消息队列
pthread_cond_t qcond = PTHREAD_COND_INITIALIZER;    // 简化初始化条件变量和互斥体
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

// 随机数范围[mi, ma]
int randint(int mi, int ma) {
    double r = (double)rand() * (1.0 / ((double)RAND_MAX + 1.0));
    r *= (double)(ma - mi) + 1.0;
    return (int)r + mi;
}

// 打印消息
void print_msg(struct msg *m) {
    printf(">>>>msg: %d\n", m->data);
}

// 压入消息
void push_msg(struct msg *m) {
    pthread_mutex_lock(&qlock);
    m->next = queue;
    queue = m;
    pthread_mutex_unlock(&qlock);
    // 通知条件满足
    pthread_cond_signal(&qcond);
}

// 生产者线程：
void* product(void *data) {
    while (1) {
        usleep(randint(1000*100, 1000*200));
        struct msg * m = malloc(sizeof(*m));
        memset(m, 0, sizeof(*m));
        m->data = randint(0, 1000);
        push_msg(m);
    }
}

// 弹出消息
struct msg* pop_msg() {
    struct msg *m;
    pthread_mutex_lock(&qlock);
    // 等待条件满足
    while (queue == NULL) pthread_cond_wait(&qcond, &qlock);
    m = queue;
    queue = m->next;
    pthread_mutex_unlock(&qlock);
    return m;
}

// 消费者线程
void* consum(void *data) {
    while (1) {
        struct msg *m = pop_msg();
        print_msg(m);
        free(m);
    }
}

int main() {
#define PRO_NUM 3
#define CON_NUM 3
    pthread_t tid_p[PRO_NUM];
    pthread_t tid_c[CON_NUM];

    int i;
    for (i = 0; i < PRO_NUM; ++i) {
        pthread_create(&tid_p[i], NULL, product, NULL);
    }
    for (i = 0; i < CON_NUM; ++i) {
        pthread_create(&tid_c[i], NULL, consum, NULL);
    }


    for (i = 0; i < PRO_NUM; ++i) {
        pthread_join(tid_p[i], NULL);
    }
    for (i = 0; i < CON_NUM; ++i) {
        pthread_join(tid_c[i], NULL);
    }
    return 0;
}