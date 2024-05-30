#include "../include/cmd_handle.h"

/*
信令包发送缓存，每个信令类型只存储1条数据，有数据需要全部发送完后再添加
信令包接收缓存，存储接收所有其他节点的不同信令类型数据，当存储到1条完整信令后接收到下一层处理
*/


all_cmd_buf_t g_tx_cmd_buf;
all_cmd_buf_t g_rx_cmd_buf[MAX_NODE_NUM];

/*****************************************************************
*接收数据判断合并、替换/添加到buf中
*
*
*****************************************************************/
void rx_cmd_frag_combine(cmd_pdu_t* cmd_pdu, UINT8* data, all_cmd_buf_t* rx_cmd_buf)
{
    cmd_buf_t* cmd_buf = &rx_cmd_buf->cmd_buf[cmd_pdu->cmd_pdu_type];
    int datalen;
    int coverflg = FALSE;

    //新数据
    if (cmd_buf->len == 0)
    {
        coverflg = TRUE;
    }
    else
    {
        //有历史数据,并且连续
        if (cmd_buf->cmd_hdr.firstpkg && cmd_buf->cmd_hdr.fragment 
            && cmd_buf->cmd_hdr.index == cmd_pdu->index && !cmd_pdu->firstpkg)
        {
            datalen = cmd_pdu->sublen - sizeof(cmd_pdu_t);
            memcpy(cmd_buf->data + cmd_buf->len, data, datalen);
            cmd_buf->len += datalen;
            cmd_buf->cmd_hdr.fragment = cmd_pdu->fragment;
        }
        else
        {
            coverflg = TRUE;
        }
    }

    if (coverflg)
    {
        //需要覆盖
        datalen = cmd_pdu->sublen - sizeof(cmd_pdu_t);
        memcpy(cmd_buf->data, data, datalen);
        cmd_buf->len = datalen;
        memcpy(&cmd_buf->cmd_hdr, cmd_pdu, sizeof(cmd_pdu_t));
    }
}

/*****************************************************************
*校验buf中信令是否完整用于接收处理
*
*
*****************************************************************/
int rx_check_cmddata_complete(UINT8 cmd_pdu_type, all_cmd_buf_t* rx_cmd_buf)
{
    cmd_buf_t* cmd_buf = &rx_cmd_buf->cmd_buf[cmd_pdu_type];

    if (cmd_buf->len && cmd_buf->cmd_hdr.firstpkg && !cmd_buf->cmd_hdr.fragment)
        return TRUE;
    return FALSE;
}

/*****************************************************************
*接收测距基值
*
*
*****************************************************************/
void rx_dist_base(all_cmd_buf_t* rx_cmd_buf)
{
    //组包
}

/*****************************************************************
*接收测距上报值
*
*
*****************************************************************/
void rx_dist_rpt(all_cmd_buf_t* rx_cmd_buf)
{
    
}

/*****************************************************************
*接收组网交互信令包
*
*
*****************************************************************/
void rx_hello(all_cmd_buf_t* rx_cmd_buf)
{
    
}

/*****************************************************************
*信令包接收处理总函数
*
*
*****************************************************************/
int rx_cmd(UINT8* macrx_buf, UINT32 len, all_cmd_buf_t* rx_cmd_buf)
{
    mac_pdu_t* mac_pdu = (mac_pdu_t*)macrx_buf;
    int rxlen = 0;
    cmd_pdu_t* cmd_pdu = NULL;
    UINT8* cmd_data = NULL;
    
    if (mac_pdu->crc != 1)
    {
        return FALSE;
    }
    
    //
    if ((mac_pdu->mac_pdu_type == 0xAAA || mac_pdu->mac_pdu_type == 0xBBB) && mac_pdu->mac_frame_len != len)
    {
        //信令包或者mac包
        return FALSE;        
    }
    if (mac_pdu->mac_pdu_type == 0xAAB && mac_pdu->mac_frame_len >= len)
    {
        //合并包
        return FALSE;        
    }

    while (rxlen < mac_pdu->mac_frame_len)
    {
        cmd_pdu = (cmd_pdu_t*)(macrx_buf + sizeof(mac_pdu_t) + rxlen);
        cmd_data = macrx_buf + sizeof(mac_pdu_t) + rxlen + sizeof(cmd_pdu_t);
        
        rx_cmd_frag_combine(cmd_pdu, cmd_data, rx_cmd_buf);
        if (!rx_check_cmddata_complete(cmd_pdu->cmd_pdu_type, rx_cmd_buf))
            continue;
        //获取信令包
        switch (cmd_pdu->cmd_pdu_type)
        {
            case MAC_NODE_HELLO:
            {
                rx_hello(rx_cmd_buf);
                break;
            }
            case DIST_BASE_DATA:
            {
                rx_dist_base(rx_cmd_buf);
                break;
            }
            case DIST_REPORT:
            {
                rx_dist_rpt(rx_cmd_buf);
                break;
            }
            default:
            {
                printf("cmd type err : %d.\n", cmd_pdu->cmd_pdu_type);
            }
        }
        rxlen += cmd_pdu->sublen;

    }
}


/*****************************************************************
*信令包发送,返回剩余字节
*
*
*****************************************************************/
int tx_cmd(UINT32 max_maclen, all_cmd_buf_t* all_cmd, UINT8* mactx_buf)
{
    int i;
    int wlen = sizeof(mac_pdu_t), sublen, mac_remainlen;
    cmd_buf_t* cmd_buf_data = NULL;
    mac_pdu_t* mac_pdu = (mac_pdu_t*)mactx_buf;
    

    //遍历发送信令包，设置状态，并计算最终剩余长度
    for (i = 0; i < MAX_CMD_TYPE; i++)
    {
        mac_remainlen = max_maclen - wlen;
        cmd_buf_data = &all_cmd->cmd_buf[i];
        if (cmd_buf_data->len)
        {
            if (mac_remainlen > CMD_HDR_LEN)
            {                
                if (cmd_buf_data->len + CMD_HDR_LEN <= mac_remainlen)
                {
                    //数据可以全部写入进去
                    sublen = cmd_buf_data->len + CMD_HDR_LEN;
                    memcpy(mactx_buf + wlen, &cmd_buf_data->cmd_hdr, sublen);
                    wlen += sublen;
                }
                else
                {
                    //数据只能写入一部分
                    sublen = mac_remainlen;
                    memcpy(mactx_buf + wlen, &cmd_buf_data->cmd_hdr, sublen);
                    wlen += sublen;
                }
                //写入此信令包，修改信令包状态
                cmd_buf_data->cmd_hdr.firstpkg = FALSE;
                cmd_buf_data->len -= (sublen - CMD_HDR_LEN);
                if (cmd_buf_data->len)
                    cmd_buf_data->cmd_hdr.fragment = TRUE;
                else
                    cmd_buf_data->cmd_hdr.fragment = FALSE;

                cmd_buf_data->cmd_hdr.sublen = sublen;
                
            }
            else
            {
                break;
            }
        }
    }

    if (wlen == sizeof(mac_pdu_t))
    {
        //无信令数据
        return max_maclen;
    }
    else
    {
        //先将mac包类型写为信令包,计算crc
        mac_pdu->mac_pdu_type = 01010101;
        mac_pdu->mac_frame_len = wlen;
        mac_pdu->crc = 1;
        return (max_maclen - mac_pdu->mac_frame_len);
    }
}

/*****************************************************************
*添加信令包数据
*
*
*****************************************************************/
void tx_add_cmd(cmd_buf_t* cmd_buf)
{
    cmd_buf->len = 100;

}

/*****************************************************************
*校验是否需要插入数据
*
*
*****************************************************************/
int tx_check_isget_cmd(cmd_buf_t* cmd_buf)
{
    if (cmd_buf->len == 0)
        return TRUE;
    return FALSE;
}

/*****************************************************************
*信令数据重置信息
*
*
*****************************************************************/
void tx_new_cmd_reset(cmd_buf_t* cmd_buf)
{
    cmd_buf->cmd_hdr.firstpkg = TRUE;
    cmd_buf->cmd_hdr.fragment = FALSE;
    cmd_buf->len = 0;
    cmd_buf->cmd_hdr.index++;
    cmd_buf->cmd_hdr.sublen = 0;
}

/*****************************************************************
*插入信令包总函数
*
*
*****************************************************************/
void tx_cmd_indata(cmd_buf_t* cmd_buf)
{
    if (tx_check_isget_cmd(cmd_buf))
    {
        tx_new_cmd_reset(cmd_buf);
        tx_add_cmd(cmd_buf);
    }
}

/*****************************************************************
*信令buf初始化
*
*
*****************************************************************/
void cmd_buf_init()
{
    int i, j;
    all_cmd_buf_t* all_cmd_buf = NULL;

    for (i = 0; i < MAX_CMD_TYPE; i++)
    {
        all_cmd_buf = &g_tx_cmd_buf;
        all_cmd_buf->cmd_buf[i].cmd_hdr.firstpkg = TRUE;
        all_cmd_buf->cmd_buf[i].cmd_hdr.fragment = FALSE;
        all_cmd_buf->cmd_buf[i].len = 0;
        all_cmd_buf->cmd_buf[i].cmd_hdr.cmd_pdu_type = i;
        all_cmd_buf->cmd_buf[i].cmd_hdr.index = 0;
        all_cmd_buf->cmd_buf[i].cmd_hdr.sublen = 0;
    }

    for (j = 0; j < MAX_NODE_NUM; j++)
    {
        all_cmd_buf = &g_rx_cmd_buf[j];
        for (i = 0; i < MAX_CMD_TYPE; i++)
        {
            all_cmd_buf->cmd_buf[i].cmd_hdr.firstpkg = FALSE;
            all_cmd_buf->cmd_buf[i].cmd_hdr.fragment = FALSE;
            all_cmd_buf->cmd_buf[i].len = 0;
            all_cmd_buf->cmd_buf[i].cmd_hdr.cmd_pdu_type = i;
            all_cmd_buf->cmd_buf[i].cmd_hdr.index = 0;
            all_cmd_buf->cmd_buf[i].cmd_hdr.sublen = 0;
        }        
    }
}


