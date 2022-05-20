//
// Created by xm on 2022/4/14.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/socket.h>

#define MAX_EVENT_NUMBER 1024
#define TCP_BUFFER_SIZE 512
#define UDP_BUFFER_SIZE 1024


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


int main(int argc,char *argv[]){
    if (argc <= 2){
        perror("arfc numbers error");
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    inet_pton(AF_INET,ip,&address.sin_addr);

    /*
     * socket TCP
     */

    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd >= 0);

    ret = bind(listenfd,(struct sockaddr *)&address,sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd,5);
    assert(ret != -1);

    /*
     * UDP socket
     * UDP 不用 listen 监听  与  accept 接收
     */
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    inet_pton(AF_INET,ip,&address.sin_addr);
    int updfd = socket(PF_INET,SOCK_DGRAM,0);
    assert(updfd >= 0);

    ret = bind(updfd,(struct sockaddr*)&address,sizeof(address));
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd,listenfd);
    addfd(epollfd,updfd);

    while (1){
        int number = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if (number < 0){
            printf("epoll failure\n");
            break;
        }

        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd){
                struct sockaddr_in clientAddress;
                socklen_t len = sizeof(clientAddress);
                int connfd = accept(listenfd,(sockaddr *)&clientAddress,&len);
                addfd(epollfd,connfd);
            } else if (sockfd == updfd){
                char buf[UDP_BUFFER_SIZE];
                memset(buf,'\0',UDP_BUFFER_SIZE);
                struct sockaddr_in clientAddress;
                socklen_t clientLen = sizeof(clientAddress);

                ret = recvfrom(updfd,buf,UDP_BUFFER_SIZE - 1,0,(struct sockaddr*)&clientAddress,&clientLen);

            }
        }
    }
}