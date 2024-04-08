#include "../include/logging.h"

log_func log_list[LOG_LEN] = {0};
char g_log_path[50] = {0};
FILE* g_log_f = NULL;
char file_buf[2048] = {0};

int main_log()
{
    char path[50] = {0};
	char date[50] = {0};
	FILE* fp = NULL;
	int val;

    fp = fopen("/A_test/debug.log", "a");
    if (fp == NULL)
    {
        PRINT_DEBUG("fopen err.\n");
        return -1;
    }
    val = ftell(fp);
    PRINT_DEBUG("seek:%d\n", val);
	if (val >= 50)
	{
		fclose(fp);
		system("mv /A_test/debug.log /A_test/debug_bak.log");
		printf("%s\n", strerror(errno));
        fp = fopen("/A_test/debug.log", "a");
        if (fp == NULL)
        {
            PRINT_DEBUG("fopen err.\n");
            return -1;
        }	
	}
	now_time(date);
	DEBUG_LOG(fp, "%s\n", date);
	fclose(fp);
    return 0;
}


void log_init(/*UINT32 log_type, */char* path)
{
    // logging_register();
    // log_handler(log_type);
    sprintf(path, "/A_test/debug_%d.log", 0);
    debug_logging(path);
}

int debug_logging(char* path)
{
    FILE* fp = NULL;
    fp = fopen(path, "a");
    if (fp == NULL)
    {
        PRINT_DEBUG("fopen err.\n");
        return -1;
    }
}

void now_time(char* date)
{
    time_t t;
    struct tm* p;
    time(&t);
    p = localtime(&t);
    sprintf(date, "%4d-%02d-%02d %02d:%02d:%02d", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

BOOL log_file_init(char* filename)
{
    g_log_f = fopen(filename, "w");
    if (g_log_f == NULL)
    {
        printf("open file %s fail.\n", filename);
        return FALSE;
    }
    return TRUE;
}

void write_file(char* buf, int len)
{
    int ret;
    ret = fwrite(buf, 1, len, g_log_f);
    if (ret != len)
    {
        printf("write file fail. ret = %d\n", ret);
    }
}

void log_file_close()
{
    if (g_log_f != NULL)
    {
        fclose(g_log_f);
        g_log_f = NULL;
    }
}

// void logging_register()
// {
//     log_list[DEBUG] = debug_logging;
//     // log_list[INFO] = info_logging;
//     // log_list[WARNING] = warning_logging;
//     // log_list[ERROR] = error_logging;
// }


// int log_handler(UINT32 log_type)
// {
//     int ret = -1;
//     log_type = log_type < LOG_LEN? log_type: ERROR;
//     ret = log_list[log_type]();
//     return ret;
// }


    
// int info_logging()
// {
//     FILE* fp = NULL;
//     fp = fopen("/A_test/debug.log", "a");
//     if (fp == NULL)
//     {
//         MY_PRINT("fopen err.\n","");
//         return -1;
//     }
// }
// int warning_logging()
// {
//     FILE* fp = NULL;
//     fp = fopen("/A_test/debug.log", "a");
//     if (fp == NULL)
//     {
//         MY_PRINT("fopen err.\n","");
//         return -1;
//     }
// }
// int error_logging()
// {
//     FILE* fp = NULL;
//     fp = fopen("/A_test/debug.log", "a");
//     if (fp == NULL)
//     {
//         MY_PRINT("fopen err.\n","");
//         return -1;
//     }
// }