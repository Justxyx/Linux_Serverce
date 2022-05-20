//
// Created by xm on 2022/4/6.
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>

#define BUFFER_SIZE 1024

static const char* status_line[2] = {"200 OK","500 Internal server error"};


int main(int argc,char *argv[]){
    if (argc <= 3){
        printf("argc nums error");
        return 1;
    }

    const char* ip = argv[1];   // ip
    int port = atoi(argv[2]);  // 端口
    const char* file_name = argv[3];     // 目标文件

    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);   // ip地址转换函数  绑定ip
    address.sin_port = htons(port);         // 绑定端口

    int sock = socket(PF_INET,SOCK_STREAM,0);   // tcp 链接sock
    assert(sock >- 0);

    int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));  // sockt 与ip端口绑定
    assert(ret != -1);

    ret = listen(sock,5);  // 开始监听
    assert(ret != -1);

    struct sockaddr_in client;  // 客户端sockt
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock,(struct sockaddr*)&client,&client_addrlength);
    if (connfd < 0){
        printf("errno is %d\n",errno);
    } else{
        char header_buf[BUFFER_SIZE];
        char *file_buf;
        struct stat file_stat;
        bool valid = true;
        int len = 0;
        if (stat(file_name,&file_stat) < 0){
            valid = false;
        } else{
            if (S_ISDIR(file_stat.st_mode)){
                valid = false;    // 目标文件是一个目录
            }else if (file_stat.st_mode & S_IROTH) // 当前用户有读取目标文件的权限
            {
                int fd = open(file_name,O_RDONLY);
                file_buf = new char[file_stat.st_size+1];
                memset(file_buf,'\0',file_stat.st_size+1);  // 初始化
                if (read(fd,file_buf,file_stat.st_size +1) < 0){
                    valid = false;
                }
            } else{
                valid = false;
            }
        }

        //如果目标文件夹有效，则发送正常的http应答
        if (valid){
            ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[0] );
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE-1-len,
                            "Content-Length: %d\r\n", file_stat.st_size );
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n" );

            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = strlen(file_buf);
            ret = writev(connfd,iv,2);
        } else{
            ret = snprintf( header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[1] );
            len += ret;
            ret = snprintf( header_buf + len, BUFFER_SIZE-1-len, "%s", "\r\n" );
            send( connfd, header_buf, strlen( header_buf ), 0 );
        }
        close(connfd);
        delete[] file_buf;
    }
    close(sock);
    return 0;
}