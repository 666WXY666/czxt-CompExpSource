/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 19:58:51
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 19:59:41
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
    __pid_t pid;
    if ((pid = fork()) > 0)
    {
        printf("I am the parent process.\n");
        /*父进程处理过程*/
    }
    else if (pid == 0)
    {
        printf("I am the child process.\n");
        /*子进程处理过程*/
    }
    else
    {
        printf("fork error\n");
        exit(0);
    }
}