#ifndef __PROCESS_INTERACTIVE__
#define __PROCESS_INTERACTIVE__

#include "../include/common.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define TEXT_SZ 2048 //申请共享内存大小
#define MAX_TEXT 512
#define BUFFER_SIZE 30

#pragma pack(1)
#pragma pack()
typedef struct shared_use_st
{
 int written_by_you; //written_by_you为1时表示有数据写入，为0时表示数据已经被消费者提走
 int data;
 char some_text[TEXT_SZ];
} stshared_use_st;


/*用于消息收发的结构体--my_msg_type：消息类型，some_text：消息正文*/
struct my_msg_st
{
 long int my_msg_type;  //消息类型必须为long类型
 char some_text[MAX_TEXT];
};

typedef void (* sig_func)(int);

/*联合体变量*/
union semun
{
   int val; //信号量初始值
   struct semid_ds *buf;
   unsigned short int *array;
   struct seminfo *__buf;
};
/******************************function start**********************************/
int customer();
int producer();

void mmap_write();
void mmap_read();

int msgqueue_send();
int msgqueue_recv();

void channel();

void sig_alarm(int sig);
void sig_handler();

/*函数声明，信号量定义*/
int createSemSet(int nums);
int initSem(int setid, int nums, int initVal);
int getSemSet(int nums);
int P(int semid, int who);
int V(int semid, int who);
int destroySemSet(int semid);
int sem_PV_handler();
/*******************************function end*********************************/


/*******************************sub process start*********************************/
#define MAX_SUB_PROC_NUM 20
enum SUB_PROC_SN
{
    SUB_PROC_0 = 0,
    SUB_PROC_1,
    SUB_PROC_2,
    SUB_PROC_3,
    SUB_PROC_4,
    SUB_PROC_NUM,
};

typedef void (*subproc_callback_func)(unsigned long data);

typedef struct
{
    BOOL valid;
    pid_t pid;
    subproc_callback_func func;
    unsigned long data;
} st_sub_proc;


void proc_register_init();
void proc_wait_handle();
/*******************************sub process end*********************************/
#endif