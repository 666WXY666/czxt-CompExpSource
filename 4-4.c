/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 15:39:17
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 17:24:13
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static int a = 5;
void *create(void *arg)
{
    printf("New pthread...\n");
    printf("Child: a = %d\n", a);
    a = -1;
    return (void *)0;
}
int main(int argc, const char *argv[])
{
    int error;
    pthread_t id1;
    a = 10;
    error = pthread_create(&id1, NULL, create, NULL);
    if (error != 0)
    {
        printf("New thread is not created!\n");
        return -1;
    }
    sleep(1);
    printf("Parent: a = %d\n", a);
    printf("New thread is created...\n");
    return 0;
}