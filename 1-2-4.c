/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-10 20:45:16
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 21:38:56
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
        int status;
        printf("I am the parent process.\n");
        sleep(3);
        kill(pid, 5);
        wait(&status);
        printf("%#X,%d,%d\n", status, WTERMSIG(status), WEXITSTATUS(status));
        /*父进程处理过程*/
    }
    else if (pid == 0)
    {
        while (1)
        {
            printf("I am the child process.\n");
            sleep(1);
        }
        /*子进程处理过程*/
        exit(0);
    }
    else
    {
        printf("fork error\n");
        exit(0);
    }
}