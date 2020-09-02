/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-10 20:45:16
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 20:33:44
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
        int status, wait_pid;
        printf("I am the parent process.\n");
        wait_pid = wait(&status);
        printf("pid=%d,wait_pid=%d\n", pid, wait_pid);
        printf("status=%#X,WTERMSIG=%d,WEXITSTATUS=%d\n", status, WTERMSIG(status), WEXITSTATUS(status));
        /*父进程处理过程*/
    }
    else if (pid == 0)
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            printf("I am the child process.\n");
            sleep(1);
        }
        /*子进程处理过程*/
        exit(5);
    }
    else
    {
        printf("fork error\n");
        exit(0);
    }
}