#ifndef __COMMON_H__
#define __COMMON_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <execinfo.h>

#define Vairable_To_STR(s)  #s
#define TRUE    (1)
#define FALSE   (0)
#define INVALID (-1)
#define ZERO    (0)
#define BOOL    unsigned char
#define UINT32  unsigned int
#define UINT8   unsigned char
#define UINT16  unsigned short
#define SOCK_BUF_LEN        (10240)
#define POOL_NUM            (5)
#define CONVERT_16(x)       ((x & 0xFF00) >> 1 + (x & 0x00FF) << 1)
#define CONVERT_32(x)       ((x & 0xFF000000) >> 3 + (x & 0x00FF0000) >> 1 + (x & 0x0000FF00) << 1 + (x & 0x000000FF) << 3)

/*-------------------------- 颜色打印 --------------------------*/
/*
'\033['：表示转义序列的开始

'm'：表示转义序列的结束

'0'：重置所有属性

'1'：设置粗体或高亮

'30-37'：设置字体色【30: 黑 31: 红 32: 绿 33: 黄 34: 蓝 35: 紫 36: 深绿 37: 白色】

'40-47'：设置背景色【40: 黑 41: 红 42: 绿 43: 黄 44: 蓝 45: 紫 46: 深绿 47: 白色】
printf("\033[1;31mThis text is in red and bold.\033[0m\n");
其中，'1' 表示加粗或高亮，'31' 表示前景色为红色，'\033[' 是转义序列的开始，'m' 是转义序列的结束，'\033[0m' 表示将属性重置为默认值。
/033[0m 关闭所有属性

/033[1m 设置高亮度

/033[4m 下划线

/033[5m 闪烁

/033[7m 反显

/033[8m 消隐

/033[30m -- /033[37m 设置前景色

/033[40m -- /033[47m 设置背景色

/033[nA 光标上移n行

/033[nB 光标下移n行

/033[nC 光标右移n行

/033[nD 光标左移n行

/033[y;xH设置光标位置

/033[2J 清屏

/033[K 清除从光标到行尾的内容

/033[s 保存光标位置

/033[u 恢复光标位置

/033[?25l 隐藏光标

/033[?25h 显示光标


*/

#define COLOR_FMT_NONE                  "\033[0m"
#define COLOR_FMT_DARK_GRAY             "\033[1;30m"    //深灰色
#define COLOR_FMT_RED                   "\033[0;31m"    //红色
#define COLOR_FMT_LIGHT_RED             "\033[1;31m"    //亮红色
#define COLOR_FMT_GREEN                 "\033[0;32m"    //绿色
#define COLOR_FMT_LIGHT_GREEN           "\033[1;32m"    //亮绿色
#define COLOR_FMT_YELLOW                "\033[0;33m"    //黄色
#define COLOR_FMT_LIGHT_YELLOW          "\033[1;33m"    //亮黄色
#define COLOR_FMT_BLUE                  "\033[0;34m"    //蓝色
#define COLOR_FMT_LIGHT_BLUE            "\033[1;34m"    //亮蓝色
#define COLOR_FMT_PURPLE                "\033[0;35m"    //紫色
#define COLOR_FMT_LIGHT_PURPLE          "\033[1;35m"    //亮紫色
#define COLOR_FMT_CYAN                  "\033[0;36m"    //青色
#define COLOR_FMT_LIGHT_CYAN            "\033[1;36m"    //亮青色
#define COLOR_FMT_LIGHT_GRAY            "\033[0;37m"    //浅灰色
#define COLOR_FMT_WHITE                 "\033[1;37m"    //白色
#define COLOR_FMT_GREEN_92_0            "\033[92;0m"    //绿色0
#define COLOR_FMT_GREEN_92_1            "\033[92;1m"    //绿色1
#define COLOR_FMT_GREEN_92_2            "\033[92;2m"    //绿色2
#define COLOR_FMT_GREEN_92_3            "\033[92;3m"    //绿色3
#define COLOR_FMT_GREEN_92_4            "\033[92;4m"    //绿色4
#define COLOR_FMT_GREEN_92_5            "\033[92;5m"    //绿色5
#define COLOR_FMT_DFT                   "\033[0m"       //默认值

/*-------------------------- 颜色打印 --------------------------*/

/*-------------------------- 组网数据 --------------------------*/
#define MAX_NODE_NUM 32

/*-------------------------- 组网数据 --------------------------*/



/**********************************function start*************************************/
void customPrint(const char* file, const char* func, int line, const char* format, ...);
UINT32 timeval2us(struct timeval BeginTime, struct timeval EndTime);
void main_dump();
int systemRet();
void setbit();
void arr2init();
void testbitmain();
void myfgets(char * data, unsigned int cnt, FILE* fd);
void update_conf();
void sort_arr();
void color();
void get_2arr();


/**********************************function end*************************************/
#define MY_PRINT(fmt, ...) (customPrint(__FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__))
#define PRINT_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__);
#define DEBUG_LOG(fp, fmt, ...) fprintf(fp, fmt, ##__VA_ARGS__);

#endif
