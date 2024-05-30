#ifndef __ROUTE_ALGO_H__
#define __ROUTE_ALGO_H__

/*
adhoc路由算法
平面结构：
分级结构：
动态源路由算法(DSR)、基于反向路径转发的拓扑分发协议(TBRPF)、优化链路状态路由算法(OLSR)、
按需距离矢量路由算法(AODV)、临时按序路由算法(TORA)、区域路由算法(ZRP)

Ad hoc路由协议，如目的序列距离矢量路由算法(DSDV)、无线路由协议(WRP)、
陆标路由协议(LANMAR)、位置辅助路由(LAR)、鱼眼状态路由算法(FSR)。
*/

#include "../include/common.h"

//接收状态
typedef struct 
{
    int nodeid;
    int txpos;
    int len;
    int overtime;
    
} st_rxslotstatus;

//接收时隙表
typedef struct 
{
    int nodeid; //本节点id
    int nf;     //本节点底噪
    
    st_rxslotstatus rxstatus[MAX_NODE_NUM]; //其他节点状态
} st_rxslottb;

//路由表
typedef struct 
{
    BOOL valid;         //
    int did;            //目的id
    int nid;            //下一跳id
    int hop;            //跳数
    int overtime;       //超时时间
    BOOL rtbreak;         //故障标记
    BOOL rtwarn;          //告警标记
} st_rtstatus;

//路由表
typedef struct 
{
    st_rtstatus    rtstatusp[MAX_NODE_NUM]; //路由状态
    
} st_rttb;

//节点参数
typedef struct 
{
    st_rttb        rtdata;
    st_rxslottb    rxslotdata;
    
} st_node_data;



















void route_main();



#endif