/*
 * @Copyright: Copyright (c) 2020 王兴宇 All Rights Reserved.
 * @Description: 
 * @Version: 
 * @Author: 王兴宇
 * @Date: 2020-05-28 16:22:53
 * @LastEditors: 王兴宇
 * @LastEditTime: 2020-07-18 10:55:17
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

#define NUM_SEMAPHORE 4 // 信号量个数
// 信号量ID
#define MUTEX_STATION 0 // 二元信号量，控制对工作台和count_A、count_B、empty的互斥访问（取/放零件）
#define SUSPEND_A 1     // 控制台无足够空位时，挂起worker A
#define SUSPEND_B 2     // 控制台无足够空位时，挂起worker B
#define SUSPEND_C 3     // 控制台无足够零件时，挂起worker C

#define SEM_KEY 0x11223344  // 信号量组的KEY
#define SHM_KEY1 0x11223355 // 共享内存的KEY1
#define SHM_KEY2 0x11223366 // 共享内存的KEY2

// 颜色
#define NONE "\e[0m"      // 复原
#define RED "\e[0;31m"    // ERROR
#define YELLOW "\e[1;33m" // WorkerA
#define PINK "\e[1;35m"   // WorkerB
#define GREEN "\e[1;32m"  // WorkerC
#define CYAN "\e[0;36m"   // Share Memory
#define BUF_SIZE 12

char *buf;
int *info; // 用于记录信息的3个整形变量
int i, sv, sem_id, shm_id, info_shm_id, C_num = 0;

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
    if (op < 0)
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
    }
    else
    {
        perror(RED "Semaphore P" NONE);
        exit(3);
    }
    return;
}
// 对信号量做减1操作，即V（signal）
void V(int sem_id, int sem_num, int op)
{
    if (op > 0)
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
    }
    else
    {
        perror(RED "Semaphore V" NONE);
        exit(3);
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
    arg[MUTEX_STATION].val = 1;
    arg[SUSPEND_A].val = 0;
    arg[SUSPEND_B].val = 0;
    arg[SUSPEND_C].val = 0;
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
    printf("\n");
    // 创建Info共享内存，用于记录信息
    info_shm_id = shmget(SHM_KEY2, 3 * sizeof(int), IPC_CREAT | IPC_EXCL | 0666);
    if (info_shm_id == -1)
    {
        perror(RED "Create Info Share Memory" NONE);
        exit(1);
    }
    printf("Create Info Share Memory: OK\n");
    // 初始化Info共享内存
    info = (int *)shmat(info_shm_id, 0, 0); // 获取指向共享内存段的指针
    if (info == (int *)-1)
    {
        perror(RED "Attach Info Share Memory" NONE);
        exit(1);
    }
    info[0] = 0;  // 当前工作台中已有的A的数量（count_A）
    info[1] = 0;  // 当前工作台中已有的B的数量（count_B）
    info[2] = 12; // 工作台中空位数量（empty=12-(count_A+count_B)）
    printf("Attach Info Share Memory: OK\n");
    // 展示info共享内存
    printf("count_A=%d, count_B=%d, empty=%d\n\n", info[0], info[1], info[2]);
    // 创建Buffer共享内存
    shm_id = shmget(SHM_KEY1, BUF_SIZE * sizeof(char), IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1)
    {
        perror(RED "Create Buffer Share Memory" NONE);
        exit(1);
    }
    printf("Create Buffer Share Memory: OK\n");
    // 初始化Buffer共享内存
    buf = (char *)shmat(shm_id, 0, 0); // 获取指向共享内存段的指针
    if (buf == (char *)-1)
    {
        perror(RED "Attach Buffer Share Memory" NONE);
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
        perror(RED "Remove Buffer Share Memory" NONE);
    else
        printf("Remove Buffer Share Memory: OK\n");
    if (shmctl(info_shm_id, IPC_RMID, 0) == -1)
        perror(RED "Remove Info Share Memory" NONE);
    else
        printf("Remove Info Share Memory: OK\n");
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
            sleep(1 + random() % 10);     // 生产2个A
            P(sem_id, MUTEX_STATION, -1); // 申请对count_A、empty和控制台的访问权
            // 还能再生产放置2个A，最多可放7+2=9个A，以便至少给B留下12-9=3个空位，防止工作台中没有足够多的B，导致worker C无法同时取出3个B
            if (info[0] <= 7 && info[2] >= 2)
            {
                // 同时放入工作台2个A
                for (i = 0; i < 2; i++)
                {
                    char *c;
                    c = strchr(buf, ' ');
                    *c = 'A';
                }
                printf(YELLOW "Worker A puts 2 As to the station\n" NONE);
                show_shm();

                info[0] += 2; //修改count_A
                info[2] -= 2; //修改empty

                // 展示info共享内存
                printf("count_A=%d, count_B=%d, empty=%d\n\n", info[0], info[1], info[2]);

                V(sem_id, MUTEX_STATION, 1); //释放对控制台、count_A和empty的访问权
                V(sem_id, SUSPEND_C, 1);     //控制台放入新零件，解挂/唤醒worker C
            }
            else
            {
                V(sem_id, MUTEX_STATION, 1); //释放对控制台和count_A、empty的访问权
                printf("Worker A suspended\n\n");
                P(sem_id, SUSPEND_A, -1); //控制台无足够空位，或不允许再放入A，转入waiting态，挂起自身
            }
        }
    }
    // WorkerB
    else if (fork() == 0)
    {
        while (1)
        {
            sleep(1 + random() % 10);     // 生产1个B
            P(sem_id, MUTEX_STATION, -1); // 申请对count_B、empty和控制台的访问权
            // 还能再生产并放置1个B，最多可放7+1=8个B，以便至少给A留下12-8=4个空位，防止工作台中没有足够多的A，导致worker C无法同时取出4个A
            if (info[1] <= 7 && info[2] >= 1)
            {
                // 放入工作台1个B
                char *c;
                c = strchr(buf, ' ');
                *c = 'B';
                printf(PINK "Worker B puts 1 B to the station\n" NONE);
                show_shm();

                info[1] += 1; //修改count_B
                info[2] -= 1; //修改empty

                // 展示info共享内存
                printf("count_A=%d, count_B=%d, empty=%d\n\n", info[0], info[1], info[2]);

                V(sem_id, MUTEX_STATION, 1); //释放对控制台、count_B和empty的访问权
                V(sem_id, SUSPEND_C, 1);     //控制台放入新零件，解挂/唤醒worker C
            }
            else
            {
                V(sem_id, MUTEX_STATION, 1); //释放对控制台和count_B、empty的访问权
                printf("Worker B suspended\n\n");
                P(sem_id, SUSPEND_B, -1); //控制台无足够空位，或不允许再放入B，转入waiting态，挂起自身
            }
        }
    }
    // WorkerC
    else if (fork() == 0)
    {
        while (1)
        {
            P(sem_id, MUTEX_STATION, -1); // 申请对count_A、count_B和控制台的访问权
            // 有足够多的A、B用于装配
            if (info[0] >= 4 && info[1] >= 3)
            {
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

                info[0] -= 4; //修改count_A
                info[1] -= 3; //修改count_B
                info[2] += 7; //修改empty

                // 展示info共享内存
                printf("count_A=%d, count_B=%d, empty=%d\n\n", info[0], info[1], info[2]);

                V(sem_id, MUTEX_STATION, 1); //释放对控制台、count_A、count_B的控制权
                V(sem_id, SUSPEND_A, 1);     //控制台新增空位，解挂A
                V(sem_id, SUSPEND_B, 1);     //控制台新增空位，解挂B

                // 生产1个C
                printf(GREEN "Worker C is producing C-%d......\n\n" NONE, ++C_num);
                sleep(1 + random() % 10);
                printf(GREEN "C-%d is produced\n\n" NONE, C_num);
            }
            else
            {
                V(sem_id, MUTEX_STATION, 1); //释放对控制台、count_A、count_B的控制权
                printf("Worker C suspended\n\n");
                P(sem_id, SUSPEND_C, -1); //控制台无足够零件，转入waiting态，挂起自身
            }
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
