/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-28 16:22:53
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-07-12 00:07:23
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define NUM_SEMAPHORE 6 // 信号量个数
// 信号量ID
#define MUTEX 0     // 用于WorkerA、WorkerB和WorkerC对Station的互斥访问
#define A_FULL 1    // 用于限制WorkerC取A
#define A_EMPTY 2   // 用于限制WorkerA生产A的数量
#define B_FULL 3    // 用于限制WorkerC取B
#define B_EMPTY 4   // 用于限制WorkerB生产B的数量
#define ALL_EMPTY 5 // 用于限制整个Station的生产数量

#define SEM_KEY 0x11223344 // 信号量组的KEY
#define SHM_KEY 0x11223355 // 共享内存的KEY

// 颜色
#define NONE "\e[0m"      // 复原
#define RED "\e[0;31m"    // ERROR
#define YELLOW "\e[1;33m" // WorkerA
#define PINK "\e[1;35m"   // WorkerB
#define GREEN "\e[1;32m"  // WorkerC
#define CYAN "\e[0;36m"   // Share Memory

#define BUF_SIZE 12 // 工作台大小

char *buf;
int i, sv, sem_id, shm_id, C_num = 0;

union semun
{
    int val;               // SETVAL使用的值
    struct semid_ds *buf;  // IPC_STAT、IPC_SET 使用缓存区
    unsigned short *array; // GETALL,、SETALL 使用的数组
    struct seminfo *__buf; // IPC_INFO(Linux特有) 使用缓存区
};
// 对信号量做减1操作，即P（wait）
void P(int sem_id, int sem_num, int op)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = op; //P()
    sem_buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_buf, 1) == -1)
    {
        perror(RED "Semaphore P" NONE);
        exit(2);
    }
    return;
}
// 对信号量做减1操作，即V（signal）
void V(int sem_id, int sem_num, int op)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = op; //V()
    sem_buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_buf, 1) == -1)
    {
        perror(RED "Semaphore V" NONE);
        exit(2);
    }
    return;
}
// 展示共享内存
void show_shm()
{
    printf(CYAN "[");
    for (i = 0; i < BUF_SIZE - 1; i++)
    {
        printf("%c,", buf[i]);
    }
    printf("%c]\n\n" NONE, buf[BUF_SIZE - 1]);
}
// 创建并初始化信号量和共享内存
void create_ipc()
{
    union semun arg[NUM_SEMAPHORE];
    arg[MUTEX].val = 1;
    arg[A_FULL].val = 0;
    arg[A_EMPTY].val = 9;
    arg[B_FULL].val = 0;
    arg[B_EMPTY].val = 8;
    arg[ALL_EMPTY].val = 12;
    // 创建信号量
    if ((sem_id = semget(SEM_KEY, NUM_SEMAPHORE, IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {
        perror(RED "Create Semaphores" NONE);
        exit(1);
    }
    printf("Create Semaphores: OK\n");
    printf("Sem_id = %d\n", sem_id);
    // 初始化信号量
    for (i = 0; i < NUM_SEMAPHORE; i++)
    {
        semctl(sem_id, i, SETVAL, arg[i]);
    }
    for (i = 0; i < NUM_SEMAPHORE; i++)
    {
        printf("The Sem[%d] = %d\n", i, semctl(sem_id, i, GETVAL, NULL));
    }
    // 创建共享内存
    shm_id = shmget(SHM_KEY, BUF_SIZE * sizeof(char), IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1)
    {
        perror(RED "Create Share Memory" NONE);
        exit(1);
    }
    printf("Create Share Memory: OK\n");
    // 初始化共享内存
    buf = (char *)shmat(shm_id, 0, 0); // 获取指向共享内存段的指针
    if (buf == (char *)-1)
    {
        perror(RED "Attach Share Memory" NONE);
        exit(1);
    }
    for (i = 0; i < BUF_SIZE; i++)
    {
        buf[i] = ' ';
    }
    printf("Initialize QUEUE: OK\n");
    show_shm();
}
// 删除信号量和共享内存
void remove_ipc()
{
    // 删除信号量
    if (semctl(sem_id, 0, IPC_RMID, 0) == -1)
        perror(RED "Remove Semaphores" NONE);
    else
        printf("Remove Semaphores: OK\n");
    // 删除共享内存
    if (shmctl(shm_id, IPC_RMID, 0) == -1)
        perror(RED "Remove Share Memory" NONE);
    else
        printf("Remove Share Memory: OK\n");
}

// CTRL-C回调函数
void sig_handler(int sig)
{
    printf("\n");
    remove_ipc();
    printf("EXIT: OK\n");
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
    // 创建并初始化信号量和共享内存
    create_ipc();
    // WorkerA
    if (fork() == 0)
    {
        while (1)
        {
            sleep(1 + random() % 10); // 生产2个A

            P(sem_id, A_EMPTY, -2);
            P(sem_id, ALL_EMPTY, -2);
            P(sem_id, MUTEX, -1);

            // 同时放入工作台2个A
            for (i = 0; i < 2; i++)
            {
                char *c;
                c = strchr(buf, ' ');
                *c = 'A';
            }
            printf(YELLOW "Worker A puts 2 As to the station\n" NONE);
            show_shm();

            V(sem_id, MUTEX, 1);
            V(sem_id, A_FULL, 2);
        }
    }
    // WorkerB
    else if (fork() == 0)
    {
        while (1)
        {
            sleep(1 + random() % 10); // 生产1个B

            P(sem_id, B_EMPTY, -1);
            P(sem_id, ALL_EMPTY, -1);
            P(sem_id, MUTEX, -1);

            // 放入工作台1个B
            char *c;
            c = strchr(buf, ' ');
            *c = 'B';
            printf(PINK "Worker B puts 1 B to the station\n" NONE);
            show_shm();

            V(sem_id, MUTEX, 1);
            V(sem_id, B_FULL, 1);
        }
    }
    // WorkerC
    else if (fork() == 0)
    {
        while (1)
        {
            P(sem_id, A_FULL, -4);
            P(sem_id, B_FULL, -3);
            P(sem_id, MUTEX, -1);

            // 同时从工作台取出4个A、3个B
            for (i = 0; i < 4; i++)
            {
                char *c;
                c = strchr(buf, 'A');
                *c = ' ';
            }
            for (i = 0; i < 3; i++)
            {
                char *c;
                c = strchr(buf, 'B');
                *c = ' ';
            }
            printf(GREEN "Worker C gets 4 As and 3 Bs from the station\n" NONE);
            show_shm();

            V(sem_id, MUTEX, 1);
            V(sem_id, ALL_EMPTY, 7);
            V(sem_id, B_EMPTY, 3);
            V(sem_id, A_EMPTY, 4);

            // 生产1个C
            printf(GREEN "Worker C is producing C-%d......\n\n" NONE, ++C_num);
            sleep(1 + random() % 10);
            printf(GREEN "C-%d is produced\n\n" NONE, C_num);
        }
    }
    // Main
    else
    {
        // CTRL-C信号自行处理
        signal(SIGINT, sig_handler);
        wait(&sv);
        wait(&sv);
        wait(&sv);
        printf("THANKS\n");
        return 0;
    }
}
