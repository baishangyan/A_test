#include "../include/common.h"


void arr2init()
{
	char a[2][2] = {[0 ... 1][0 ... 1]=11};	//1=ARR_LEN - 1
        printf("[%d,%d,%d,%d]\n", a[0][0], a[0][1], a[1][0], a[1][1]);
}


void customPrint(const char* file, const char* func, int line, const char* format, ...)
{
    va_list val;
    char ret[1024] = {0};
    printf("[%s:%s line:%d]", file, func, line);
    va_start(val, format);

    vsprintf(ret, format, val);
    va_end(val);
    printf("%s", ret);
}

unsigned int timeval2us(struct timeval BeginTime, struct timeval EndTime)
{
    unsigned int uiUs;
    uiUs = (EndTime.tv_sec - BeginTime.tv_sec) * 1000000 + (EndTime.tv_usec - BeginTime.tv_usec);
    return uiUs;
}

void setbit()
{
	unsigned char a = 0;
	a |= (1 << 7);
	printf("[%d]\n", a);
	a &= ~(1 << 7);
	printf("%d\n", a);
}

int systemRet()
{
	pid_t stat = system("cp -r /workspace/master/linux_kernal ./tmp");	//会fork一个进程执行命令

	printf("--------------\n");

	if (-1 == stat)
	{
		printf("返回-1，fork失败，或者waitpid返回除了EINTR之外的错误\n");
		return 0;
	}
	else
	{
		printf("exit status value = [0x%x]\n", stat);
		if (WIFEXITED(stat))
		{
			if (0 == WEXITSTATUS(stat))
			{
				printf("0 == WEXITSTATUS(stat), 运行完毕\n");
			}
			else
			{
				printf("0 != WEXITSTATUS(stat), script exit code %d\n", WEXITSTATUS(stat));
				return 0;
			}
		}
		else
		{
			printf("WIFEXITED(stat) 为假，exit status = [%d]\n", WIFEXITED(stat));
			return 0;
		}
	}
	return 0;
}


/***********************************************************************************
-rdynamic,#注意这里不加-rdynamic的话,backtrace显示的只有地址,不能显示地址对应的符号
./main(+0x1223) 
下面我们把Makefile里的
LINKFLAGS += -rdynamic
再解屏,看看效果
可以看到生成的不是地址了,而是./main(函数符号+偏移地址)
./main(_Z14dump_backtracei+0x35)

# 这部分代码里加了注释,下面的signal意思是注册一个信号,当发生该信号时,回调后面的函数,
# 下面是注册了三个信号, 
# SIGSEGV是段错误,这错误就是指针相关的问题一般会引起该错误,比如取一个空指针的值
# SIGABRT是当程序用了abort()或者assert之类的函数时候会触发
# SIGFPE也就是除0时候发生的了

然后我们再看一下dump_backtrace函数
#当程序崩溃后,我们把收到的信号值打了出来,
#然后用signal(signum,SIG_DFL)让程序默认处理该信号
#接着用myfunc3把调用栈打印了出来,实际项目中我们可以把这些信息写到日志里
*************************************************************************************/
void myfunc3(void){
    int j, nptrs;
    void *buffer[100];
    char **strings;
 
    nptrs = backtrace(buffer, 100);
    printf("backtrace() returned %d addresses\n", nptrs);
 
    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */
 
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
 
    for (j = 0; j < nptrs; j++){
        printf("%s\n", strings[j]);
    }
 
    free(strings);
}
 
void dump_backtrace(int signum){
    printf("dump_backtrace on signal:%d\n", signum);
    signal(signum,SIG_DFL);
    myfunc3();
}
//测试段错误函数
void test_sig_segv(){
    //这个就是段错误了
    int* p = NULL;
    printf("*p = %d\n", *p);
}

void main_dump()
{ 
    int pid = getpid();
    printf("pid %d\n", pid);
    //这里是先注册信号，当程序运行出错的时候，
    //捕捉到信号后打印调用栈，
    //实际使用中也是这种办法
    signal(SIGSEGV, dump_backtrace); //segmentation violation
    signal(SIGABRT, dump_backtrace); //abort program (formerly SIGIOT)
    signal(SIGFPE, dump_backtrace); //floating-point exception
 
    // test_sig_segv();
}


int process_task()
{
	int pid;
	pid = fork();

	//process_task();
	//channel();
	//sig_handler();
	//sem_PV_handler();
	if (pid == 0)
	{
		
		//producer();
		// mmap_write();
		//msgqueue_send();

	}
	else if (pid > 0)
	{
		
		// mmap_read();
		//customer();
		//msgqueue_recv();
	}
	else
	{
		printf("fork fail.\n");
		return 2;	
	}
	return 1;
}


/*
int task_sock()
{
	int udpfd, udpMultifd, tcpClientfd, tcpServerfd;
	int i, j, count = 20;
	struct sockaddr_in sin, remotefd;
	UINT8* pbuf = g_sockBufsend;
	char strIp[20];
	int sendLen, recvLen;
	printf("compile time:%s %s...\n", __DATE__, __TIME__);

	sendLen = generate_msg(pbuf);
	//udpfd = init_udp(10000);
	//udpMultifd = init_udpmulti(10000);
	//tcpClientfd = init_tcpclient(20000);
	tcpServerfd = init_tcpserver(10000);
	g_tcpServer_crol.sockfd = tcpServerfd;

	init_dst_addr(&sin);

	inet_ntop(AF_INET, &sin.sin_addr, strIp, sizeof(strIp));
	printf("dst : ip:%s, port:%d...\n", strIp, ntohs(sin.sin_port));

	if (tcpServerfd != INVALID)
	{
		while (TRUE)
		{
			usleep(200);
			//recv_tcpserver(g_sockBufrecv, &g_tcpServer_crol);
			send_tcpserver(pbuf, sendLen, &g_tcpServer_crol);
			//rw_tcpserver(pbuf, g_sockBufrecv, sendLen, &g_tcpServer_crol);
		}
		
	}
	


	if (tcpClientfd != INVALID)
	{
		// tcpclient接收
		while (TRUE)
		{
			usleep(200);
			recvLen = recv_tcpclient(tcpClientfd, &sin, g_sockBufrecv);
			if (recvLen > 0)
			{
				for (j = 0; j < recvLen; j++)
				{
					printf("%02X ", g_sockBufrecv[j]);
				}
				printf("\n");
			}
			
		}
		// tcpclient发送
		for (i = 0; i < count; i++)
		{
			sleep(1);
			send_tcpclient(tcpClientfd, &sin, pbuf, sendLen);
			printf("..................%d....................\n", i);
		}
		// tcpClient收发
		while (TRUE)
		{
			usleep(200);
			recvLen = rw_tcpclient(tcpClientfd, &sin, sendLen, pbuf, g_sockBufrecv);
			if (recvLen > 0)
			{
				for (j = 0; j < recvLen; j++)
				{
					printf("%02X ", g_sockBufrecv[j]);
				}
				printf("\n");
			}	
		}
		
	}

	if (udpMultifd != INVALID)
	{
		// udpmulti接收
		while (TRUE)
		{
			usleep(200);
			recvLen = recv_udp_or_udpmulti(udpMultifd, &remotefd, g_sockBufrecv);
			if (recvLen > 0)
			{
				for (j = 0; j < recvLen; j++)
				{
					printf("%02X ", g_sockBufrecv[j]);
				}
				printf("\n");
			}
			
		}
		
		// udpmulti发送
		for (i = 0; i < count; i++)
		{
			sleep(1);
			send_udp_or_udpmulti(udpMultifd, pbuf, sendLen, &sin);
			printf("..................%d....................\n", i);
		}
	}
	if (udpfd != INVALID)
	{
		// udp接收
		while (TRUE)
		{
			usleep(200);
			recvLen = recv_udp_or_udpmulti(udpfd, &remotefd, g_sockBufrecv);
			if (recvLen > 0)
			{
				for (j = 0; j < recvLen; j++)
				{
					printf("%02X ", g_sockBufrecv[j]);
				}
				printf("\n");
			}
			
		}
		
		// udp发送
		for (i = 0; i < count; i++)
		{
			sleep(1);
			send_udp_or_udpmulti(udpfd, pbuf, sendLen, &sin);
			printf("..................%d....................\n", i);
		}
		
	}
	
	return 1;
}
*/
void head_tail_cut_char(char* str, char ch)
{
	int len = strlen(str);
	if (*str == ch)
	{
		memcpy(str, str + 1, len + 1);
	}
	len = strlen(str);
	if(*(str + len - 1) == ch)
	{
		*(str + len - 1) = '\0';
	}
}

//去掉首尾字符
void cut_char()
{
	char str[10] = ",11,";
	printf("%s\n", str);
	head_tail_cut_char(str, ',');
	printf("%s\n", str);
}

typedef struct ttt_
{
	int a;
	int c;
	int b;
} ttt;

//结构体初始化一部分为0
void struct_init_0()
{
	ttt tt;
	tt.a = 1;tt.b = 2;tt.c = 3;
	printf("%d,%d,%d\n", tt.a,tt.b, tt.c);
	memset(&tt, 0, (void*)&tt.c - (void*)&tt.a);
	printf("%d,%d,%d\n", tt.a, tt.b, tt.c);
}

//msg打印到文件
#define msg_printf(format,...)  do {\
	if (g_msg_fp == NULL) break;\
	fprintf(g_msg_fp, format, ##__VA_ARGS__);\
} while(0)

FILE* g_msg_fp = NULL;

void to_file_init()
{
	g_msg_fp = fopen("./msg.log", "w");
	setbuf(g_msg_fp, NULL);
	if (g_msg_fp == NULL)
	{
		printf("fopen debug file err.\n");
	}
}

//测试位域
typedef struct bit_uint
{
	unsigned int uint1 : 8;
	unsigned int uint2 : 8;
	unsigned int uint3 : 8;
	unsigned int uint4 : 8;

    unsigned char u8_1 : 4;
    unsigned char u8_2 : 4;        
} bit_uint_t;
typedef struct bit_uchar
{
    unsigned char u8_1 : 4;
    unsigned char u8_2 : 4;        
} bit_uchar_t;

typedef struct veriable_arr
{
    unsigned char u8;
    unsigned char a[];
} veriable_arr_t;


void testbitmain()
{
	bit_uint_t bit_uint_val;
    bit_uchar_t bit_uchar_val;
	unsigned int val, val1;

	bit_uint_val.uint1 = 0xaa;
	bit_uint_val.uint2 = 0xbb;
	bit_uint_val.uint3 = 0xcc;
	bit_uint_val.uint4 = 0xdd;

    bit_uchar_val.u8_1 = 0x1;
    bit_uchar_val.u8_2 = 0x0;

	val = *((unsigned int*)(&bit_uint_val));
    val1 = *((unsigned char*)(&bit_uchar_val));

	printf("-0x%x, 0x%x-\n", val, val1);
}

//测试结构体位域

/*
#define _GNU_SOURCE
#include <sched.h>
//进程绑定cpu
cpu_set_t mask;
int cpun;
CPU_ZERO(&mask);
CPU_SET(1, &mask);
if (-1 == sched_setaffinity(0, sizeof(cpu_set_t), &mask))
{
		printf("set affinity err.\n");
}
cpun = sched_getcpu();
printf("current cpu:%d\n", cpun);

//测试读取excel文件，需要先将excel文件另存为csv文件
//C语言要读取xls/xlsx格式文件比较复杂，可以先把xls/xlsx格式另存为csv格式。
// #include <stdio.h>
 
int read_excel_main()
{
    char filename[] = "i.csv";
    char table[2][3][40];
 
    FILE* fp = fopen(filename,"r");     // fp指向文件头部
    for(int i = 0 ; i < 2 ; i++)
        for(int j = 0 ; j < 3 ; j++){
            fscanf(fp,"%[^,\n]",table[i][j]);  // 读到逗号或\n为止
            fgetc(fp); // 读取一个字符(逗号或\n)
        }
 
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < 3 ; j++)
            printf("%-20s",table[i][j]);   // -20:左对齐,占20个字符宽度
        printf("\n");
    }
    fclose(fp);
    return 0;
}

*/

void myfgets(char * data, unsigned int cnt, FILE* fd)
{
    fgets(data, cnt, fd);
    char* find = strchr(data, '\n');
    if (find)
        *find = '\0';
    
}

void test_abc()
{
    int a;
	char b;
	unsigned char c;
	b = -5;
	c = b;
	a = (char)c;
	printf("a = %d.\n", a);	
}

#define DHCP_CONF_FILE "./udhcpd.conf"
#define FILE_BUFF_LEN (1000)

//修改配置文件参数
void update_conf(int netseg)
{
    int idx;
    char tmpbuff[FILE_BUFF_LEN] = {0};
    FILE *fp = fopen( DHCP_CONF_FILE, "r+");
    char key[100];
    char val[100];

    if (!fp)
    {
        printf("open file null.\n");
    }
    while(fgets(tmpbuff, FILE_BUFF_LEN, fp))
    {
        strcpy(key, "start ");
        sprintf(val, "192.168.%d.40", netseg);
        if (0 == strncmp(tmpbuff, key, strlen(key))) 
        {
            fseek(fp, -1 * (strlen(tmpbuff) - strlen(key)), SEEK_CUR);
            fwrite(val, strlen(val), 1, fp);
        }

        strcpy(key, "end ");
        sprintf(val, "192.168.%d.254", netseg);
        if (0 == strncmp(tmpbuff, key, strlen(key))) 
        {
            fseek(fp, -1 * (strlen(tmpbuff) - strlen(key)), SEEK_CUR);
            fwrite(val, strlen(val), 1, fp);
        }

        strcpy(key, "opt router ");
        sprintf(val, "192.168.%d.1", netseg);
        if (0 == strncmp(tmpbuff, key, strlen(key))) 
        {
            fseek(fp, -1 * (strlen(tmpbuff) - strlen(key)), SEEK_CUR);
            fwrite(val, strlen(val), 1, fp);
        }
    }
    fclose(fp);
    system("sync");
}

//打印颜色
void color()
{
    printf("%sthis is test.%s\n", COLOR_FMT_LIGHT_BLUE, COLOR_FMT_DFT);
}

//排序
void sort_arr()
{
    int i, j, n, tmp;

    int arr[5] = {5, 3, 4, 1, 2};
    n = 5;
    
    for (i = 0; i < (n - 1); i++)
    {
        for (j = 1; j < (n - i); j++)
        {
            if (arr[j - 1] > arr[j])
            {
                memcpy(&tmp, &arr[j], sizeof(tmp));
                memcpy(&arr[j], &arr[j - 1], sizeof(tmp));
                memcpy(&arr[j - 1], &tmp, sizeof(tmp));
            }
        }
    }
    printf("[%d, %d, %d, %d, %d]\n", arr[0], arr[1], arr[2], arr[3], arr[4]);
}

//sscanf常用正则用法
void use_sscanf()
{
    char buf[32];

    //*: 可用于格式中, (即 %*d 和 %*s) 加了星号 (*) 表示跳过此数据不读入
    //[^a]: 匹配非a的任意字符，贪婪性
    
    sscanf("123456 ", "%s", buf);
    printf("%s/n", buf);

    sscanf("123456 ", "%4s", buf); //取长度为4
    printf("%s/n", buf);

    sscanf("123456 abcdedf", "%[^ ]", buf); //遇到空格就停止
    printf("%s/n", buf);

    sscanf("123456abcdedfABCDEF", "%[1-9a-z]", buf); //取数字和小写字母
    printf("%s/n", buf);

    sscanf("123456abcdedfBCDEF", "%[^A-Z]", buf); //取遇到大写字母为止的字符串
    printf("%s/n", buf);

    sscanf("iios/1234ABCD@122", "%*[^/]/%[^@]", buf); //先将 "iios/"过滤掉，再将非'@'的一串内容送到buf中
    printf("%s/n", buf);

    sscanf("hello, world", "%*s%s", buf); //,后面有个空格
    printf("%s/n", buf);

    
}

//字符串拼接
void str_combine()
{
    static char s_ptr[1000] = {0};
    unsigned int len;
    char *nf = "{\"Noisefloor\":[";
    char *ptr = s_ptr;
    char tmp[100] = {0};
    int noiseNum = 22;

    strcpy(tmp, nf);
    len = strlen(tmp);
    memcpy(ptr, tmp, len);
    ptr += len;
    memset(tmp, 0x00, sizeof(tmp));

    sprintf(tmp, "%d,", noiseNum);
    len = strlen(tmp);
    memcpy(ptr, tmp, len);
    ptr += len;
    memset(tmp, 0x00, sizeof(tmp));

    printf("%s\n", s_ptr);

}


//二维数组申请内存
void get_2arr()
{
    int** arr = NULL;
    int i;
    unsigned char aaa[4][4];

    arr = (int**)malloc(sizeof(int*) * 2);

    for (i = 0; i < 2; i++)
    {
        arr[i] = (int*)malloc(sizeof(int) * 3);
    }

    arr[1][2] = 10;

    printf("arr item val:%d.\n", arr[1][2]);

    for(i =0; i < 2; i++)
        free(arr[i]);
    free(arr);
    arr = NULL;

    memset(aaa, 100, sizeof(aaa));
    printf("arr val:%d.\n", aaa[1][2]);
}
