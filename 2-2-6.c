/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-12 18:07:04
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-12 18:23:07
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define THREAD_NUMBER 3        //线程数
#define REPEAT_NUMBER 5        //每个线程中的小任务数
#define DELAY_TIME_LEVELS 10.0 //小任务之间的最大时间间隔

void *thrd_func(void *arg)
{
    long thrd_num = (long)arg;
    int delay_time = 0;
    int count = 0;

    printf("Thread %ld is starting\n", thrd_num);
    for (count = 0; count < REPEAT_NUMBER; count++)
    {
        delay_time = (int)(rand() * DELAY_TIME_LEVELS / (RAND_MAX)) + 1;
        sleep(delay_time);
        printf("\tThread %ld:job %d delay=%d\n", thrd_num, count, delay_time);
    }
    printf("Thread %ld finished\n", thrd_num);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t thread[THREAD_NUMBER];
    long no, res;
    void *thrd_ret;
    srand(time(NULL));
    for (no = 0; no < THREAD_NUMBER; no++)
    {
        res = pthread_create(&thread[no], NULL, thrd_func, (void *)no);
        if (res != 0)
        {
            printf("Create thread %d failed\n", no);
            exit(res);
        }
    }

    printf("Creating threads success\nWaiting for thread to finish...\n");
    for (no = 0; no < THREAD_NUMBER; no++)
    {
        res = pthread_join(thread[no], &thrd_ret);
        if (!res)
        {
            printf("Thread %d joined\n", no);
        }
        else
        {
            printf("Thread %d joined failed\n", no);
        }
    }

    return 0;
}
