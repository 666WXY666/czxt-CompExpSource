/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 16:41:25
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 17:19:25
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *mythread1(void *arg)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("I am thread 1\n");
        sleep(2);
    }
}

void *mythread2(void *arg)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("I am thread 2\n");
        sleep(2);
    }
}

int main(int argc, char const *argv[])
{
    pthread_t id1, id2;
    int res;

    res = pthread_create(&id1, NULL, mythread1, NULL);
    if (res)
    {
        printf("Create pthread error!\n");
        return -1;
    }

    res = pthread_create(&id2, NULL, mythread2, NULL);
    if (res)
    {
        printf("Create pthread error!\n");
        return -2;
    }

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    return 0;
}
