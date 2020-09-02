/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 17:43:39
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 18:01:42
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *clean(void *arg)
{
    printf("Cleanup %s\n", (char *)arg);
    return (void *)0;
}

void *thr_fn1(void *arg)
{
    printf("Thread1 start\n");
    pthread_cleanup_push((void *)clean, "Thread1 first handler");
    pthread_cleanup_push((void *)clean, "Thread1 second handler");
    printf("Thread1 push complete\n");
    if (arg)
    {
        return ((void *)1);
    }
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return (void *)2;
}

void *thr_fn2(void *arg)
{
    printf("Thread2 start\n");
    pthread_cleanup_push((void *)clean, "Thread2 first handler");
    pthread_cleanup_push((void *)clean, "Thread2 second handler");
    printf("Thread2 push complete\n");
    if (arg)
    {
        return ((void *)3);
    }
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
    return (void *)4;
}

int main(int argc, char const *argv[])
{
    int res;
    pthread_t tid1, tid2;
    void *tret;
    res = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
    if (res != 0)
    {
        printf("Create theate error...\n");
        return -1;
    }
    res = pthread_create(&tid2, NULL, thr_fn2, (void *)0);
    if (res != 0)
    {
        printf("Create theate error...\n");
        return -1;
    }
    res = pthread_join(tid1, &tret);
    if (res != 0)
    {
        printf("Thread_join error...\n");
        return -1;
    }
    printf("Thread1 exit code %ld\n", (long)tret);
    res = pthread_join(tid2, &tret);
    if (res != 0)
    {
        printf("Thread_join error...\n");
        return -1;
    }
    printf("Thread2 exit code %ld\n", (long)tret);
    return 0;
}
