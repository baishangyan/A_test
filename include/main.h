#ifndef __MAIN_H__
#define __MAIN_H__
#include "common.h"
#include"../include/protocol_socket.h"
#include"../include/collectData.h"
#include "../include/process_interactive.h"
#include "../include/driver_module.h"
#include "../include/logging.h"
#include "../include/route_algo.h"

//#include "../include/ws.h"

/*************************引用外部start*****************************/
extern UINT8 g_sockBufrecv[SOCK_BUF_LEN];
extern UINT8 g_sockBufsend[SOCK_BUF_LEN];
extern sttcpserver_crol g_tcpServer_crol;
extern stcollectBuf g_collBuf;

/*************************引用外部end*****************************/


/*************************func start*****************************/
int process_task();

/*************************func end*****************************/
#endif
