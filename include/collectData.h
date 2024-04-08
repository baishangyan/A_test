#ifndef __COLLECTDATA_H__
#define __COLLECTDATA_H__
#include"../include/common.h"

enum ecollectDataType
{
    eCollect_noise = 0,
    eCollect_dpfs,
    eCollect_end
};

typedef int (* collectType)();
typedef struct collectBuf
{
    UINT32 count;
    int len;
    char data[1000];
} stcollectBuf;





/******************************function start********************************/
void collect(int type, int count);
void init_register_callback();
int collectNoise(FILE* fp);
int collectDbfs(FILE* fp);
void thread_func();
/*******************************function end*******************************/
#endif

