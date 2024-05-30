#include "../include/protocol_socket.h"


UINT8 g_sockBufrecv[SOCK_BUF_LEN] = {0};
UINT8 g_sockBufsend[SOCK_BUF_LEN] = {0};
sttcpserver_crol g_tcpServer_crol;
/*********************************************************************
 * functaion  : init_udp
 * description: 初始化UDP单播socket
 * ******************************************************************/
int init_udp(int port)
{
    int sockfd = INVALID;
    struct sockaddr_in sin;
    int val;

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID)
    {
        printf("sockfd err.\n");
        return INVALID;
    }
    
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &val, sizeof(int));
    val = bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
    if (val < 0)
    {
        printf("++++++%d++++++%d+++++\n", val, port);
        close(sockfd);
        return INVALID;
    }
    return sockfd;
}

/*********************************************************************
 * functaion  : recv_udp
 * description: udp接收
 * ******************************************************************/
int recv_udp_or_udpmulti(int localfd, struct sockaddr_in* remotefd, UINT8* pbuf)
{
    int recvLen = ZERO;
    UINT32 sin_len = sizeof(struct sockaddr_in);
    recvLen = recvfrom(localfd, g_sockBufrecv, sizeof(g_sockBufrecv), MSG_DONTWAIT, (struct sockaddr*)remotefd, &sin_len);
    return recvLen;
}

/*********************************************************************
 * functaion  : send_udp
 * description: udp发送
 * ******************************************************************/
void send_udp_or_udpmulti(int localfd, UINT8* pbuf, UINT32 len, struct sockaddr_in* remotefd)
{
    sendto(localfd, pbuf, len, 0, (struct sockaddr*)remotefd, sizeof(struct sockaddr_in));
}

/*********************************************************************
 * functaion  : init_udpmulti
 * description: 初始化UDP组播socket
 * ******************************************************************/
int init_udpmulti(int port, const char* mulip)
{
    int sockfd = INVALID;
    struct ip_mreq mreq;
    struct sockaddr_in sin;
    UINT8 conf = 0;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, mulip, &sin.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID)
    {
        printf("sockfd err,\n"); 
        return INVALID;
    }

    if (bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) < 0)
    {
        printf("bind err.\n");
        close(sockfd);
        return INVALID;
    }
    bzero(&mreq, sizeof(struct ip_mreq));
    bcopy((void*)&sin.sin_addr.s_addr, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &conf, sizeof(conf));

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
    {
        printf("multi membership err.\n");
        close(sockfd);
        return INVALID;
    }
    return sockfd;
}

/*********************************************************************
 * functaion  : init_tcpserver
 * description: 初始化TCP服务端
 * ******************************************************************/
int init_tcpserver(int port)
{
    struct sockaddr_in addr;
    int sockfd = INVALID;
    int confVal = 1;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // if (inet_pton(AF_INET, "addr", &addr.sin_addr) <= 0)
    // {
    //     MY_PRINT("tcp server ip invalid.\n", "");
    // }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID)
    {
        MY_PRINT("tcp sock:%d create failed.\n", port);
        return INVALID;
    }
    printf("tcp server sockcreate success.\n");
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &confVal, sizeof(confVal));
    if ((bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) != 0)
    {
        MY_PRINT("tcp server sock bind failed.\n", "");
        close(sockfd);
        return INVALID;
    }
    else
    {
        printf("tcp server sock bind success.\n");
        if ((listen(sockfd, 5)) != 0)
        {
            MY_PRINT("tcp listen failed.\n", "");
            close(sockfd);
            return INVALID;
        }
        else
        {
            printf("tcp server listen success.\n");
        }
    }
    return sockfd;
}

/*********************************************************************
 * functaion  : init_tcpclient
 * description: 初始化TCP客户端
 * tcp客户端可以不绑定客户端的sockfd对应ip、port
 * ******************************************************************/
int init_tcpclient(int port)
{
    struct sockaddr_in addr;
    int sockfd = INVALID;
    int connfd = INVALID;
    int confVal = 1;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (inet_pton(AF_INET, "192.168.10.43", &addr.sin_addr) <= 0)
    {
        MY_PRINT("tcp server ip invalid.\n", "");
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID)
    {
        MY_PRINT("tcp sock:%d create failed.\n", port);
        return INVALID;
    }
    printf("tcp client sock create success.\n");
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &confVal, sizeof(confVal));
    
    connfd = connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    return sockfd;
}

/*********************************************************************
 * functaion  : rw_tcpclient
 * description: TCP客户端接收和发送
 * ******************************************************************/
int tcpconnect(int localfd, struct sockaddr_in* serversin)
{
    int connfd;
    connfd = connect(localfd, (struct sockaddr*)serversin, sizeof(struct sockaddr_in));
    return INVALID;
}

/*********************************************************************
 * functaion  : rw_tcpclient
 * description: TCP客户端接收和发送
 * ******************************************************************/
int rw_tcpclient(int localfd, struct sockaddr_in* serversin, int sendLen, UINT8* psendBuf, UINT8* precvBuf)
{
    int rLen = 0;
    int optval;
    int optlen = sizeof(optval);
    if (0 == getsockopt(localfd, SOL_SOCKET, SO_ERROR, (char*)&optval, (socklen_t*)&optlen))
    {
        if (0 == optval)
        {
            if (sendLen > 0)
            {
                if (write(localfd, psendBuf, sendLen) < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                {
                    tcpconnect(localfd, serversin);
                }
            }
            rLen = read(localfd, precvBuf, SOCK_BUF_LEN);
            if (rLen > 0)
            {
                //接收到数据
            }
            else if (rLen < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
            {
                tcpconnect(localfd, serversin);
            }
        }
        else if (optval != EALREADY && optval != EINPROGRESS)
        {
            tcpconnect(localfd, serversin);
        }
    }
    else
    {
        tcpconnect(localfd, serversin);
    }
    return rLen;
}

/*********************************************************************
 * functaion  : r_tcpclient
 * description: TCP客户端接收
 * ******************************************************************/
int recv_tcpclient(int localfd, struct sockaddr_in* serversin, UINT8* pbuf)
{
    int rLen = 0;
    int optval;
    int optlen = sizeof(optval);
    if (0 == getsockopt(localfd, SOL_SOCKET, SO_ERROR, (char*)&optval, (socklen_t*)&optlen))
    {
        if (0 == optval)
        {
            rLen = read(localfd, pbuf, SOCK_BUF_LEN);
            if (rLen < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
            {
                tcpconnect(localfd, serversin);
            }
        }
        else if (optval != EALREADY && optval != EINPROGRESS)
        {
            tcpconnect(localfd, serversin);
        }
    }
    else
    {
        tcpconnect(localfd, serversin);
    }
    return rLen;
}

/*********************************************************************
 * functaion  : send_tcpclient
 * description: TCP客户端发送
 * ******************************************************************/
void send_tcpclient(int localfd, struct sockaddr_in* serversin, UINT8* pbuf, int sendLen)
{
    int optval;
    int optlen = sizeof(optval);
    if (0 == getsockopt(localfd, SOL_SOCKET, SO_ERROR, (char*)&optval, (socklen_t*)&optlen))
    {
        if (0 == optval)
        {
            if (sendLen > 0)
            {
                printf("errno:%s.\n", strerror(errno));
                if (write(localfd, pbuf, sendLen) < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                {
                    printf("---3---\n");
                    tcpconnect(localfd, serversin);
                }
            }
        }
        else if (optval != EALREADY && optval != EINPROGRESS)
        {
            tcpconnect(localfd, serversin);
        }
    }
    else
    {
        tcpconnect(localfd, serversin);
    }
}

/*********************************************************************
 * functaion  : rw_tcpserver
 * description: TCP服务端收发
 * ******************************************************************/
void rw_tcpserver(UINT8* psendBuf, UINT8* precvBuf, int sendLen, sttcpserver_crol* crol)
{
    int recvLen = 0;
    unsigned int i, j;
    int connfd = 0;
    int ret = -1;
    int connMax = 0;
    struct timeval timeout; //超时时间

    acceptTcpConnet(crol);
    connMax = crol->connfd_alivenum;

    //超时时间设置
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    crol->select_read_set = crol->read_set;
    crol->select_write_set = crol->write_set;
    //select函数会修改select_read_set和select_write_set集合，存在事件的保存，没有事件的删除；
    //所以需要留存read_set和write_set供后面使用,因为accept接入后是长连接，没有close挥手就不会再接入同一个sockfd而填充FD集合
    ret = select(crol->max_fd + 1, &(crol->select_read_set), &(crol->select_write_set), NULL, &timeout);
    if (ret <= 0)
    {
        return;
    }
    else
    {
        if (FD_ISSET(crol->sockfd, &(crol->select_read_set)))
        {
            acceptTcpConnet(crol);
        }
        
        for (i = 0; i < connMax; i++)
        {
            connfd = crol->connfd_alive[i];
            if (FD_ISSET(connfd, &(crol->select_write_set)))
            {
                if (sendLen > 0)
                {
                    if (write(connfd, psendBuf, sendLen) < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                    {
                        close(connfd);
                        crol->connfd_alive[i] = 0;
                        crol->connfd_alivenum--;
                        FD_CLR(connfd, &(crol->read_set));
                        FD_CLR(connfd, &(crol->write_set));
                        continue;
                    }
                }
            }

            if (FD_ISSET(connfd, &(crol->select_read_set)))
            {
                recvLen = read(connfd, precvBuf, SOCK_BUF_LEN);
                if (recvLen > 0)
                {
                    for (j = 0; j < recvLen; j++)
                    {
                        printf("%02x ", precvBuf[j]);
                    }
                    printf("\n");
                    
                }
                else if (recvLen < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                {
                    close(connfd);
                    crol->connfd_alive[i] = 0;
                    crol->connfd_alivenum--;
                    FD_CLR(connfd, &(crol->read_set));
                    FD_CLR(connfd, &(crol->write_set));
                }
            }
        }
    }
}

/*********************************************************************
 * functaion  : send_tcpserver
 * description: TCP服务端收发
 * ******************************************************************/
void send_tcpserver(UINT8* psendBuf, int sendLen, sttcpserver_crol* crol)
{
    unsigned int i;
    int connfd = 0;
    int ret = -1;
    int connMax = 0;
    struct timeval timeout; //超时时间

    acceptTcpConnet(crol);
    connMax = crol->connfd_alivenum;

    //超时时间设置
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    crol->select_write_set = crol->write_set;
    //select函数会修改select_read_set和select_write_set集合，存在事件的保存，没有事件的删除；
    //所以需要留存read_set和write_set供后面使用,因为accept接入后是长连接，没有close挥手就不会再接入同一个sockfd而填充FD集合
    ret = select(crol->max_fd + 1, NULL, &(crol->select_write_set), NULL, &timeout);
    if (ret <= 0)
    {
        return;
    }
    else
    {   
        for (i = 0; i < connMax; i++)
        {
            connfd = crol->connfd_alive[i];
            if (FD_ISSET(connfd, &(crol->select_write_set)))
            {
                if (sendLen > 0)
                {
                    if (write(connfd, psendBuf, sendLen) < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                    {
                        close(connfd);
                        crol->connfd_alive[i] = 0;
                        crol->connfd_alivenum--;
                        FD_CLR(connfd, &(crol->read_set));
                        FD_CLR(connfd, &(crol->write_set));
                    }
                }
            }
        }
    }
}

/*********************************************************************
 * functaion  : recv_tcpserver
 * description: TCP服务端收发
 * ******************************************************************/
void recv_tcpserver(UINT8* precvBuf, sttcpserver_crol* crol)
{
    int recvLen = 0;
    unsigned int i, j;
    int connfd = 0;
    int ret = -1;
    int connMax = 0;
    struct timeval timeout; //超时时间

    acceptTcpConnet(crol);
    connMax = crol->connfd_alivenum;

    //超时时间设置
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    crol->select_read_set = crol->read_set;
    //select函数会修改select_read_set和select_write_set集合，存在事件的保存，没有事件的删除；
    //所以需要留存read_set和write_set供后面使用,因为accept接入后是长连接，没有close挥手就不会再接入同一个sockfd而填充FD集合
    ret = select(crol->max_fd + 1, &(crol->select_read_set), NULL, NULL, &timeout);
    if (ret <= 0)
    {
        return;
    }
    else
    {
        if (FD_ISSET(crol->sockfd, &(crol->select_read_set)))
        {
            acceptTcpConnet(crol);
        }
        
        for (i = 0; i < connMax; i++)
        {
            connfd = crol->connfd_alive[i];

            if (FD_ISSET(connfd, &(crol->select_read_set)))
            {
                recvLen = read(connfd, precvBuf, SOCK_BUF_LEN);
                if (recvLen > 0)
                {
                    for (j = 0; j < recvLen; j++)
                    {
                        printf("%02x ", precvBuf[j]);
                    }
                    printf("\n");
                    
                }
                else if (recvLen < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)
                {
                    close(connfd);
                    crol->connfd_alive[i] = 0;
                    crol->connfd_alivenum--;
                    FD_CLR(connfd, &(crol->read_set));
                    FD_CLR(connfd, &(crol->write_set));
                }
            }
        }
    }
}


/*********************************************************************
 * functaion  : acceptTcpConnet
 * description: 服务端接入tcp客户端连接
 * ******************************************************************/
int acceptTcpConnet(sttcpserver_crol *crol)
{
    int connfd = -1;
    struct sockaddr_in cli;
    int len = sizeof(cli);
    unsigned int i;
    if (crol->connfd_alivenum >= UART_TCP_CLIENT_MAX)
    {
        return 0;
    }
    connfd = accept(crol->sockfd, (struct sockaddr*)&cli, (socklen_t *)&len);
    if (connfd < 0)
    {
        // printf("server acccept failed...\n");
    }
    else
    {
        // set socket nonblock
        fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL, 0) | O_NONBLOCK);
        for (i = 0; i < UART_TCP_CLIENT_MAX; i++)
        {
            if (crol->connfd_alive[i] == 0)
            {
                crol->connfd_alive[crol->connfd_alivenum] = connfd;
                crol->connfd_alivenum++;
                FD_SET(connfd, &(crol->read_set));
                FD_SET(connfd, &(crol->write_set));
                if (connfd > crol->max_fd)
                {
                    crol->max_fd = connfd;
                }
                break;
            }
        }
    }
}

/*********************************************************************
 * functaion  : generate_msg
 * description: 生成报文数据
 * ******************************************************************/
int generate_msg(UINT8* pbuf)
{
    stframe_send* psend = (stframe_send*)pbuf;
	psend->head = FRAME_HEAD;
	psend->tail = FRAME_TAIL;
	memset(psend->data, 0x0a, sizeof(psend->data));
    return sizeof(stframe_send);
}

/*********************************************************************
 * functaion  : init_dst_addr
 * description: 初始化目的地地址
 * ******************************************************************/
void init_dst_addr(struct sockaddr_in* psin)
{
    psin->sin_family = AF_INET;
    psin->sin_port = htons(20000);
    inet_pton(AF_INET, "192.168.10.43", &psin->sin_addr);
}


//////////////////////////////////////////////////////////////////
//自定义组播协议

multi_t g_multi[e_multi_p_max];
int g_multi_fd = -1;
databuf_t g_databuf[e_databufmax];


//测距基值发送
void dist_basedata_snd(void* data)
{
    g_databuf[e_tx].len += 1;



    memcpy(g_databuf[e_tx].buf, data, g_databuf[e_tx].len);
}

//测距基值接收计算距离
void dist_basedata_rcv()
{
    
}

//非ssid测距数据发送
void dist_len_snd(void* data)
{
    g_databuf[e_tx].len += 1;



    memcpy(g_databuf[e_tx].buf, data, g_databuf[e_tx].len);
}

//ssid测距数据收集
void dist_len_rcv()
{
    
}

//ssid测距数据上报
void dist_len_rpt()
{
    
}

int check_p_hdr_til(UINT8* buf, int len, int dataidx)
{
    pdu_hdr_t* pdu_hdr = (pdu_hdr_t*)&buf[dataidx];
    pdu_til_t* pdu_til = NULL;

    if (pdu_hdr->head == MULTI_HEAD)
    {
        if ((pdu_hdr->len + dataidx) <= len)
        {
            pdu_til = (pdu_til_t*)&buf[dataidx + pdu_hdr->len - sizeof(pdu_til_t)];
            if (pdu_til->tail == MULTI_TAIL)
            {
                return pdu_hdr->len;
            }
            else
            {
                printf("tail fail : %#x.\n", pdu_til->tail);
            }
        }
        else
        {
            printf("len fail : %#x.\n", pdu_hdr->len);
        }
    }
    else
    {
        printf("head fail : %#x.\n", pdu_hdr->head);
    }

    return FALSE;
}

void multi_pdu_rx(UINT8* buf)
{
    pdu_hdr_t* pdu_hdr = (pdu_hdr_t*)buf;
    int sdu_len = pdu_hdr->len - (sizeof(pdu_hdr_t) + sizeof(pdu_til_t));

    switch(pdu_hdr->msg_type)
    {
        case e_dist_basedata:
        {
            if (sdu_len == sizeof(MacTWRHellodata))
            {
                dist_basedata_rcv();
            }
            else
            {
                printf("dist basedata rxlen err : %d.\n", sdu_len);
            }
            break;
        }
        case e_dist_len:
        {
            if (sdu_len == sizeof(MacNodeLendata))
            {
                dist_len_rcv();
            }
            else
            {
                printf("dist basedata rxlen err : %d.\n", sdu_len);
            }
            break;
        }
        default:
        {
            printf("dist msg_type err : %d.\n", pdu_hdr->msg_type);
        }
    }
}

//组播数据接收，拆分出数据包
void multi_data_rx(UINT8* buf, int len)
{
    int dataidx = 0;
    int pkglen, msgnum = 0;

    while(dataidx < len)
    {
        //校验包头包尾
        pkglen = check_p_hdr_til(buf, len, dataidx);
        if (!pkglen)
        {
            printf("loss pkg len : %d.\n", (len - dataidx));
            break;
        }
        multi_pdu_rx(&buf[dataidx]);

        dataidx += pkglen;
    }
}

//组播遍历接收
void multi_rx()
{
    int recvLen = 0;
    UINT32 sin_len = sizeof(struct sockaddr_in);
    struct sockaddr_in remotesin;

    
    remotesin.sin_family = AF_INET;
    remotesin.sin_port = htons(MULTI_PORT);
    inet_pton(AF_INET, "225.0.0.30", &remotesin.sin_addr);
    
    recvLen = recvfrom(g_multi_fd, g_databuf[e_tx].buf, sizeof(g_databuf[e_tx].buf), MSG_DONTWAIT, (struct sockaddr*)&remotesin, &sin_len);

    if (recvLen > 0)
    {
        g_databuf[e_tx].len = recvLen;
        multi_data_rx(g_databuf[e_tx].buf, g_databuf[e_tx].len);
    }
}


//组播遍历发送
void multi_tx()
{
    int i;
    struct sockaddr_in remotesin;
    remotesin.sin_family = AF_INET;
    remotesin.sin_port = htons(MULTI_PORT);
    inet_pton(AF_INET, "225.0.0.30", &remotesin.sin_addr);

    g_databuf[e_tx].len = 0;
    //将数据收集到buf
    for (i = 0; i < e_multi_p_max; i++)
    {
        if (g_multi[i].flg)
        {
            g_multi[i].func(g_multi[i].data);
        }
    }

    //统一发送buf
    sendto(g_multi_fd, g_databuf[e_tx].buf, g_databuf[e_tx].len, 0, (struct sockaddr*)&remotesin, sizeof(struct sockaddr_in));
}

//组播初始化
void dist_init()
{
    multi_t* multi_p = NULL;
    
    g_multi_fd = init_udpmulti(MULTI_PORT, "225.0.0.30");

    multi_p = &g_multi[e_dist_basedata];
    multi_p->flg = FALSE;
    multi_p->data = NULL;
    multi_p->func = dist_basedata_snd;
    multi_p->expire = 10000;

    multi_p = &g_multi[e_dist_len];
    multi_p->flg = FALSE;
    multi_p->data = NULL;
    multi_p->func = dist_len_snd;
    multi_p->expire = 10000;
    

    g_databuf[e_tx].len = 0;
    g_databuf[e_rx].len = 0;
}

//定时处理
void expire_handle(int icount)
{
    multi_t* multi_p = NULL;
    int i;

    for (i = 0; i < e_multi_p_max; i++)
    {
        multi_p = &g_multi[i];
        if (icount % multi_p->expire == 100)
        {
            multi_p->flg = TRUE;
        }
    }

    
}

