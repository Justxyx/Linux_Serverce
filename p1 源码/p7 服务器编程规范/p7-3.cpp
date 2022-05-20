//
// Created by xm on 2022/5/11.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


bool daemonize(){
    // 创建子进程 关闭父进程 这样可以使得程序在后台运行
    pid_t pid = fork();
    if (pid < 0)
        return false;
    else if (pid > 0)
        exit(0);

    // 设置文件权限掩码  一般配合create file 使用
    umask(0);

    // 新建会话 设置本进程为进程组的首领
    pid_t sid = setsid();

    // 切换工作目录
    if ((chdir("/")) < 0){
        return false;
    }

    // 关闭标准输入 标准输出 标准错误输出
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 将标准输入 输出 error 重定向到 /dev/null 文件
    open("/dev/null",O_RDONLY);
    open("/dev/null",O_RDWR);
    open("/dev/null",O_RDWR);
}