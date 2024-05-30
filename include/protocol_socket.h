#ifndef __PROTOCOL_SOCKET__
#define __PROTOCOL_SOCKET__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common.h"

#define UART_TCP_CLIENT_MAX     (16)
#define FRAME_HEAD              (0x0FF0)
#define FRAME_TAIL              (0xE00E)

typedef struct tcpserver_crol
{
    int connfd_alivenum;
    int max_fd;
    fd_set read_set;
    fd_set write_set;
    fd_set select_read_set;
    fd_set select_write_set;
    int sockfd;
    int connfd_alive[UART_TCP_CLIENT_MAX];
    
} sttcpserver_crol;

typedef struct frame_send
{
    UINT16      head;
    UINT8       data[15];
    UINT16      tail;
} stframe_send;

//////////////////////////////////////////////////////////////////
//自定义组播协议

typedef struct 
{
#define MULTI_HEAD 0xFB00
    UINT16 head;
    UINT16 len;
    UINT16 msg_type;
} pdu_hdr_t;

typedef struct 
{
#define MULTI_TAIL 0x00BF
    UINT16 tail;
} pdu_til_t;

//测距数据
typedef struct {
    int replyb;
    int roundb;
} MacTWRLendata;

//测距基值传递
typedef struct {
    int nodeid;
    MacTWRLendata twrdata[MAX_NODE_NUM];
} MacTWRHellodata;

//测距数据上报主节点
typedef struct {
    unsigned char nodeid;
    char num;
    char tmp[2];
    char rxpwr[MAX_NODE_NUM];
    int len[MAX_NODE_NUM];
    int len2[MAX_NODE_NUM];
} MacNodeLendata;

//主节点测距显示
typedef struct {
    unsigned char num;
    unsigned char nextid;
    char tmp[2];
    MacNodeLendata lendata[MAX_NODE_NUM];
} MacHelloNodeLendata;


//服务
typedef struct 
{

} sdu_msg_t;

enum E_MULTI_P_TYPE 
{
    e_dist_basedata = 0,
    e_dist_len,
    e_multi_p_max
};

typedef void (*multi_func_t)(void*);

typedef struct 
{
    int flg;
    int expire;
    int cnt;
    void* data;
    multi_func_t func;
} multi_t;

typedef struct 
{
    int len;
    UINT8 buf[SOCK_BUF_LEN];
} databuf_t;
enum {e_tx, e_rx, e_databufmax};

#define MULTI_PORT 8008



//////////////////////////////////////////////////////////////////

/*************************功能函数start*************************/
int init_udp(int port);
int init_udpmulti(int port, const char* mulip);

int recv_udp_or_udpmulti(int localfd, struct sockaddr_in* remotefd, UINT8* pbuf);
void send_udp_or_udpmulti(int localfd, UINT8* pbuf, UINT32 len, struct sockaddr_in* remotefd);

int init_tcpclient(int port);
int tcpconnect(int localfd, struct sockaddr_in* serversin);
void send_tcpclient(int localfd, struct sockaddr_in* serversin, UINT8* pbuf, int sendLen);
int recv_tcpclient(int localfd, struct sockaddr_in* serversin, UINT8* pbuf);
int rw_tcpclient(int localfd, struct sockaddr_in* serversin, int sendLen, UINT8* psendBuf, UINT8* precvBuf);

int init_tcpserver(int port);
int acceptTcpConnet(sttcpserver_crol *crol);
void recv_tcpserver(UINT8* precvBuf, sttcpserver_crol* crol);
void send_tcpserver(UINT8* psendBuf, int sendLen, sttcpserver_crol* crol);
void rw_tcpserver(UINT8* psendBuf, UINT8* precvBuf, int sendLen, sttcpserver_crol* crol);

int generate_msg(UINT8* pbuf);
void init_dst_addr(struct sockaddr_in* psin);

/*************************功能函数end*************************/
#endif
