#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char *argv[]){
    if (argc <= 2)
        return 1;
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&server_address.sin_addr);
    server_address.sin_port = htons(port);

    int socketfd = socket(PF_INET,SOCK_STREAM,0);
    assert(socketfd > 0);

    if (connect(socketfd,(struct  sockaddr*)&server_address, sizeof(server_address)) < 0){
        printf("connection failed\n");
    } else{
        const char *s1 = "abc";
        const char *s2 = "123";

        send(socketfd,s1, strlen(s1),0);
        send(socketfd,s2, strlen(s2),0);
    }
    close(socketfd);
    return 0;
}