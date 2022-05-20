//
// Created by xm on 2022/4/14.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
static int pipefd[2];

int setnonblocking(int fd){
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

void addfd(int epollfd,int fd){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);
}

/*
 * 信号处理函数
 */
void sig_handler(int sig){
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1],(char*)&msg,1,0);
}

void addsig(int sig){
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;

    // 在处理信号中时  阻塞所有 所有的信号
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL) != -1);

}


int main(int argc,char *argv[]){
    if (argc <= 2){
        perror("argc error");
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET,ip,&address.sin_addr);

    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd >= 0);

    ret = bind(listenfd,(struct sockaddr*) &address,sizeof(address));
    if (ret == -1){
        perror("bind error");
        return 1;
    }

    ret = listen(listenfd,5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd,listenfd);

    /*
     * 创建管道，将pipefd[0]设置为可读事件
     */
    ret = socketpair(PF_UNIX,SOCK_STREAM,0,pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd,pipefd[0]);

    addsig(SIGHUP);
    addsig(SIGCHLD);
    addsig(SIGTERM);
    addsig(SIGINT);
    bool stop_server = false;

    while (!stop_server){
        int number = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if (number < 0)
            break;
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd){
                struct sockaddr_in client_address;
                socklen_t  client_len = sizeof(client_address);
                int connfd = accept(listenfd,(struct sockaddr*) &client_address,&client_len);
                addfd(epollfd,connfd);
            }

            /*
             * 如果发现就绪的文件描述符是pipefd[0],则处理信号
             */
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)){
                int sig;
                char signals[1024];
                ret = recv(pipefd[0],signals,sizeof(signals),0);
                if (ret == -1)
                    continue;
                else{
                    /*
                     * 因为每个信号值占一个字节 所以按字节来逐个接收信号
                     */
                    for (int j = 0; j < ret; ++j) {
                        switch (signals[i]) {
                            case SIGCHLD:
                            case SIGHUP:
                            case SIGTERM:
                            case SIGINT:{
                                stop_server = true;
                            }
                        }
                    }
                }
            } else{

            }


        }
    }

    printf("close fds\n");
    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    return 0;
}













