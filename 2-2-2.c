/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 17:06:17
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 17:29:17
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *create(void *arg)
{
    printf("New thread is create...\n");
    pthread_exit((void *)6);
}

int main(int argc, char const *argv[])
{
    pthread_t tid;
    int res;
    void *temp;
    res = pthread_create(&tid, NULL, create, NULL);
    printf("I am the main thread!\n");
    if (res)
    {
        printf("thread id not create...\n");
        return -1;
    }

    res = pthread_join(tid, &temp);
    if (res)
    {
        printf("thread is not exit...\n");
        return -2;
    }
    printf("Thread is exit code %ld \n", (long)temp);
    return 0;
}
