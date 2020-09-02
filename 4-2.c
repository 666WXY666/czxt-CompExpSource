/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 15:37:53
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 15:45:43
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *create(void *arg)
{
    char *str;
    str = (char *)arg;
    printf("The parameter passed from main is %s\n", str);
    return (void *)0;
}

int main()
{
    int error;
    pthread_t id1;
    char *str1 = "Hello!";
    char *attr = str1;
    error = pthread_create(&id1, NULL, create, (void *)attr);
    if (error != 0)
    {
        printf("This pthread is not created!\n");
        return -1;
    }
    sleep(1);
    printf("pthread is created..\n");
    return 0;
}