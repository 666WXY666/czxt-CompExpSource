/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-10 20:45:16
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 20:05:07
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
    char *envp[] = {"PATH=/tmp", "USER = 1ei", "STATUS = testing", NULL};
    char *argv_execv[] = {"echo", "excuted by execv", NULL};
    char *argv_execvp[] = {"echo", "excuted by execvp", NULL};
    char *argv_execve[] = {"env", NULL};
    if (fork() == 0)
    {
        if (execl("/bin/echo", "echo", "executed by execl", NULL) < 0)
            perror("Err on execl");
    }
    if (fork() == 0)
    {
        if (execlp("echo", "echo", "executed by execlp", NULL) < 0)
            perror("Err on execlp");
    }
    if (fork() == 0)
    {
        if (execle("/usr/bin/env", "env", NULL, envp) < 0)
            perror("Err on execle");
    }
    if (fork() == 0)
    {
        if (execv("/bin/echo", argv_execv) < 0)
            perror("Err on execv");
    }
    if (fork() == 0)
    {
        if (execvp("echo", argv_execvp) < 0)
            perror("Err on execvp");
    }
    if (fork() == 0)
    {
        if (execve("/usr/bin/env", argv_execve, envp) < 0)
            perror("Err on execve");
    }
}