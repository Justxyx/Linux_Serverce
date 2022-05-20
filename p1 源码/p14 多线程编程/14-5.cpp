//
// Created by xm on 2022/5/17.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/*
 * 用一个线程处理所有信号
 */

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void *sig_thread(void *arg){
    sigset_t *set = (sigset_t *) arg;
    int s ,sig ;
    for(;;){
        s = sigwait(set,&sig);
        if (s != 0)
            handle_error_en(s,"sigwait");
        printf("Signal handing thread get signal &d\n",sig);
    }
}

int main(){
    pthread_t thread;
    sigset_t set;
    int s;

    // 在主线程中设置信号掩码
    sigemptyset(&set);
    sigaddset(&set,SIGQUIT);
    sigaddset(&set,SIGUSR1);  // 用户定义的信号
    s = pthread_sigmask(SIG_BLOCK,&set,NULL);
    if (s != 0){
        handle_error_en(s,"pthread_sigmask");
    }
    s = pthread_create(&thread,NULL,&sig_thread,(void *)&set);

    pause();
}