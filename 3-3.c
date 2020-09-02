/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 11:52:54
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 11:56:04
 */
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int pid; /*定义进程变量*/
int main(int argc, char const *argv[])
{
    int fd[2];
    char outpipe[100], inpipe[100]; /*定义两个字符数组*/
    pipe(fd);                       /*创建一个管道*/
    while ((pid = fork()) == -1)
        ; /*如果进程创建不成功，则空循环*/
    if (pid == 0)
    {                                                         /*如果子进程创建成功，pid为进程号*/
        lockf(fd[1], 1, 0);                                   /*锁定管道*/
        sprintf(outpipe, "child process is sending message"); /*把串放入数组outpipe中*/
        write(fd[1], outpipe, 50);                            /*向管道写长为50字节的串*/
        sleep(5);                                             /*自我阻塞5秒*/
        lockf(fd[1], 0, 0);                                   /*解除管道的锁定*/
        exit(0);
    }
    else
    {
        wait(0);                 /*等待子进程结束*/
        read(fd[0], inpipe, 50); /*从管道中读长为50字节的串*/
        printf("%s\n", inpipe);  /*显示读出的数据*/
        exit(0);                 /*父进程结束*/
    }
    return 0;
}