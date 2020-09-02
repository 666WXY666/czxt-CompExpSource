/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-17 15:30:18
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-05-17 17:17:11
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
struct member
{
    int a;
    char *s;
};
void *create(void *arg)
{
    struct member *temp;
    temp = (struct member *)arg;
    printf("member->a = %d\n", temp->a);
    printf("member->s = %s\n", temp->s);
    return (void *)0;
}
int main(int argc, char const *argv[])
{
    int error;
    pthread_t id1;
    struct member *p;
    p = (struct member *)malloc(sizeof(struct member));
    p->a = 1;
    p->s = "Robben!";
    error = pthread_create(&id1, NULL, create, (void *)p);
    if (error)
    {
        printf("pthread is not created!\n");
        return -1;
    }
    sleep(1);
    printf("pthread is created!\n");
    free(p);
    p = NULL;
    return 0;
}