#include "../include/collectData.h"
#include "../include/common.h"

collectType collectFuncArr[eCollect_end] = {0};
stcollectBuf g_collBuf;

/*********************************************************************
 * functaion  : collect
 * description: 收集回调函数
 * ******************************************************************/
void collect(int type, int count)
{
    FILE* fp = NULL;
    init_register_callback();
    if (type >= eCollect_end)
    {
        MY_PRINT("type err.\n","");
        return;
    }
    fp = fopen("/A_test/collect.log", "w");
    if (fp == NULL)
    {
        MY_PRINT("fopen err.\n","");
        return;
    }
    memset(&g_collBuf, 0x00, sizeof(stcollectBuf));
    g_collBuf.count = count;    
    
    printf("****************collect start********************\n");
    while (g_collBuf.count)
    {
        //strcpy(g_collBuf.data, "hello\n");
        
        // sprintf(g_collBuf.data, "hello_%d\n", g_collBuf.count);
        // g_collBuf.len = strlen(g_collBuf.data);
        fprintf(fp, "hello\n");

        sleep(1);

        printf("count:%d, len:%d.\n", g_collBuf.count, g_collBuf.len);
        if (g_collBuf.len > ZERO)
        {
            collectFuncArr[type](fp);
        }
        g_collBuf.count--;
        if (g_collBuf.count == ZERO)
        {
            printf("****************collect end********************\n");
            break;
        }
    }
    
    
    
    fclose(fp);
    fp = NULL;
}

/*********************************************************************
 * functaion  : init_register_callback
 * description: 回调函数注册
 * ******************************************************************/
void init_register_callback()
{
    collectFuncArr[eCollect_noise] = collectNoise;
    collectFuncArr[eCollect_dpfs] = collectDbfs;
}



/*********************************************************************
 * functaion  : collectNoise
 * description: 收集底噪回调函数
 * ******************************************************************/
int collectNoise(FILE* fp)
{
    int i;
    
    fwrite(g_collBuf.data, g_collBuf.len, 1, fp);
    
    return 1;
}

/*********************************************************************
 * functaion  : collect
 * description: 收集回调函数
 * ******************************************************************/
int collectDbfs(FILE* fp)
{
    //fprintf(fp, "", );
    return 1;
}

/*********************************************************************
 * functaion  : thread_func
 * description: 线程处理函数
 * ******************************************************************/
void thread_func()
{
    printf("thread_func start.\n");
    collect(eCollect_noise, 3);
}

int thread_collect()
{
	int i, status, num = 0;
	char arr[80] = {0};
	
	pthread_t thread1;
	int threadNum = 1;
	printf("compile time:%s %s...\n", __DATE__, __TIME__);

	//pthread_join()函数还可以利用其第二个参数，得到线程函数的返回值。

	status = pthread_create(&thread1, NULL, (void*)thread_func, (void*)&threadNum);
	if (status != 0)
	{
		printf("pthread create err,code:%d.\n", status);
		return -1;
	}
	for (i = 0; i < 80; i++)
	{
		arr[i] = i;
	}
	
	
	
	while (TRUE)
	{	
		if (g_collBuf.count > ZERO)
		{
			memcpy(g_collBuf.data+g_collBuf.len, arr, 80);
			g_collBuf.len += 80;
			g_collBuf.count--;
			if (g_collBuf.count == ZERO)
			{
				break;
			}
			
		}	
	}
	pthread_join(thread1, NULL);
	

	return 1;
}