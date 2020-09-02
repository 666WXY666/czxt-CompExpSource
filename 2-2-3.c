/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 17:22:43
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 17:30:10
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *thread(void *str)
{
    int i;
    for (i = 0; i < 4; ++i)
    {
        sleep(2);
        printf("This is the thread:%d\n", i);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t pth;
    long i;
    int ret = pthread_create(&pth, NULL, thread, (void *)(i));
    pthread_join(pth, NULL);
    printf("123\n");
    for (i = 0; i < 3; ++i)
    {
        sleep(1);
        printf("This is the main:%ld\n", i);
    }
    return 0;
}