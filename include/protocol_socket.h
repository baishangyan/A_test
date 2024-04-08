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


/*************************功能函数start*************************/
int init_udp(int port);
int init_udpmulti(int port);
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