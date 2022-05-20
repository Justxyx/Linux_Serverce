#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static bool stop = false;
static void handle_term(int sig){
    stop = true;
}

int main(int argc,char*argv[]){
    signal(SIGTERM,handle_term);
    if (argc <= 3){
        printf("--");
        return 1;
    }

    const char* ip = argv[1];  // ip
    int port = atoi(argv[2]);  // 字符串变为整形  端口
    int backlog = atoi(argv[3]); // 最大连接数

    int sock = socket(PF_INET,SOCK_STREAM,0);
    assert( sock >= 0);

    // 创建一个ipv4 地址
    struct sockaddr_in address;
    bzero(&address,sizeof(address));    // The  bzero() function erases the data in the n bytes of the memory starting at the location pointed to by s
    address.sin_family = AF_INET;           // TCP/IPv4协议族
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port); // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.

    int ret = bind(sock,(struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock,backlog);
    assert(ret != -1);


    // 循环等待链接，直到有SIGTERM 信号将他中断
    while (!stop){
        sleep(1);
    }

    close(sock);
    return 0;
}