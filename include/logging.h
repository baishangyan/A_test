#ifndef __LOGGING__
#define __LOGGING__
#include"../include/common.h"

enum LOGGING
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    LOG_LEN
};
typedef int (*log_func)();




/********************************************************/
extern char file_buf[2048];
extern char g_log_path[50];
void log_init(/*UINT32 log_type, */char* path);
void now_time(char* date);
int debug_logging();
int main_log();
void write_file(char* buf, int len);
BOOL log_file_init(char* filename);
// void logging_register();
// int info_logging();
// int warning_logging();
// int error_logging();
/********************************************************/

#define printf_file(fmt, ...) {\
    sprintf(file_buf, fmt, ##__VA_ARGS__);\
    write_file(file_buf, strlen(file_buf));}
#define printf_debug(fmt, ...) fprintf(stdout, "%s[%d] %s : "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)







#endif