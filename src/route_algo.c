#include "../include/route_algo.h"
#include "../include/process_interactive.h"



/*
    通过多进程/多线程模拟多点分布式数据
    此分布式数据用于多点信息获取打桩
*/



//主函数,启动多进程模拟
void route_main()
{
    proc_register_init();

    proc_wait_handle();
}

//节点注册
void node_register(int nodeid, st_node_data* data)
{
    
}

//节点删除
void node_delete(int nodeid, st_node_data* data)
{
    
}

//节点参数修改
void update_node_data(int nodeid, st_node_data* data)
{
    
}















