//
// Created by xm on 2022/4/20.
//

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

pthread_mutex_t mutex;


// 子线程运行函数，它先获得mutex锁  5s后释放锁
void *another(void* arg){
    printf("in child thread, lock th mutex\n");
    pthread_mutex_lock(&mutex);
    sleep(5);
    pthread_mutex_unlock(&mutex);
}

int main(){
    pthread_mutex_init(&mutex,NULL);
    pthread_t id;
    pthread_create(&id,NULL,another,NULL);
    sleep(5);

    int pid = fork();
    if (pid < 0){
        perror("fork error");
        return 1;
    } else if (pid == 0){
        printf("i am child process , I want got lock\n");
        // 子进程从父进程继承了额互斥锁mutex的状态，该互斥锁处于锁住的状态
        // 是由父进程中的子线程锁住的  因此 下面的锁会一直处于阻塞 ???  其实并没有一直阻塞 不知道书上的例子为什么会这样写
        pthread_mutex_lock(&mutex);
        printf(" i can not run to here ？");
        pthread_mutex_unlock(&mutex);
        exit(0);
    } else{
        wait(NULL);
    }
    pthread_join(id,NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}