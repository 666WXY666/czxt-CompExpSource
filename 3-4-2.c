/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 12:02:43
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 15:18:06
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
void stop()
{
    printf("\npid=%d,interruption\n", getpid());
}
int main(int argc, char const *argv[])
{
    int pid1, pid2;
    signal(3, stop); // 或者signal(14, stop);
    while ((pid1 = fork()) == -1)
        ;
    if (pid1 > 0)
    {
        while ((pid2 = fork()) == -1)
            ;
        if (pid2 > 0)
        {
            sleep(3);
            kill(pid1, 16);
            kill(pid2, 17);
            wait(0);
            wait(0);
            printf("\n Parent process is killed! !\n");
            exit(0);
        }
        else
        {
            signal(17, stop);
            printf("\n Child process 2 is killed by parent!!\n");
            sleep(5);
            exit(0);
        }
    }
    else
    {
        signal(16, stop);
        printf("\n Child process 1 is killed by parent!!\n");
        sleep(5);
        exit(0);
    }
    return 0;
}
