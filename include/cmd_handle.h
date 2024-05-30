#ifndef __CMD_HANDLE_H__
#define __CMD_HANDLE_H__

#include "common.h"

#define HELLO_LEN 0x1000

enum E_CMD_TYPE {
    MAC_NODE_HELLO = 0,
    DIST_BASE_DATA,
    DIST_REPORT,
    MAX_CMD_TYPE
};

typedef struct 
{
    UINT8  cmd_pdu_type;        //信令包类型
    UINT8  firstpkg : 1;        //是否首包
    UINT8  fragment : 1;        //后面是否分段
    UINT8  index : 6;           //整包索引
    UINT16 sublen;              //子包长度
} cmd_pdu_t;


typedef struct {
    //本地维护数据
    UINT32 len;      //发端为剩余长度，收端为接收长度

    //发送接收数据
    cmd_pdu_t cmd_hdr;
    char data[HELLO_LEN];   //数据buf，存在未发数据则不再填充
} cmd_buf_t;

typedef struct 
{
    UINT32 mac_pdu_type;        //mac类型
    UINT16 mac_frame_len;       //mac信令包长度
    UINT16 crc;
} mac_pdu_t;



#define CMD_HDR_LEN (sizeof(cmd_pdu_t))

typedef struct 
{
    cmd_buf_t cmd_buf[MAX_CMD_TYPE];
} all_cmd_buf_t;



















/*-------------------------ref func start------------------------------*/


/*-------------------------ref func start------------------------------*/
#endif
