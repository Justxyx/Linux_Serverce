//
// Created by xm on 2022/5/10.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

int main(int argc,char *argv[]){
    assert(argc == 2);
    char *host = argv[1];

    // 1. 获取主机地址信息
    struct hostent *hostinfo = gethostbyname(host);
    assert(hostinfo);

    // 2. 获取daytime服务信息
    struct servent *servinfo = getservbyname("daytimne","tcp");
    printf("daytime port is %d\n",ntohs(servinfo->s_port));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = servinfo->s_port;
    address.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    int result = connect(sockfd,(struct sockaddr*)&address,sizeof(address));
    char buffer[128];
    result = read(sockfd,buffer,sizeof(buffer));
    printf("the day time is %s",buffer);
    close(sockfd);
    return 0;
}