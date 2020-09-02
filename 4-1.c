/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 15:21:43
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 15:25:17
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
void *create(void *arg)
{
    int *num;
    num = (int *)arg;
    printf("Create parameter is %d\n", *num);
    return (void *)0;
}
int main(int argc, const char *argv[])
{
    pthread_t id1;
    int error;
    int test = 4;
    int *attr = &test;
    error = pthread_create(&id1, NULL, create, (void *)attr);
    if (error)
    {
        printf("Pthread_create is not created!\n");
        return -1;
    }
    sleep(1);
    printf("Pthread_create is created..\n");
    return 0;
}