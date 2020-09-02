/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 17:32:50
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 17:50:23
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *create(void *arg)
{
    printf("New thread...\n");
    printf("This thread's id is %u \n", (unsigned int)pthread_self());
    printf("This thread process pid is %d\n", getpid());
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t tid;
    int res;
    printf("Main thread is starting...\n");
    res = pthread_create(&tid, NULL, create, NULL);
    if (res)
    {
        printf("thread id not create...\n");
        return -1;
    }
    printf("The main process pid is %d\n", getpid());
    sleep(1);
    return 0;
}
