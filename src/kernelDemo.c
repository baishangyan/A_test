
// //定时器运用，获取当前运行CPU
// static struct timer_list test_timer;
// void func1(unsigned long data)
// {
// 	int processorid = -1;

// 	processorid = raw_smp_processor_id();
// 	printk("task1_tick:%d.\n", processorid);
// 	if ((jiffies % 5) == 0)
// 	{
// 		while(1){};
// 	}
	
// }
// void func2(unsigned long data)
// {
// 	int processorid = -1;

// 	processorid = raw_smp_processor_id();
// 	printk("task2_tick:%d.\n", processorid);
// 	if ((jiffies % 5) == 0)
// 	{
// 		while(1){};
// 	}
// }
// mod_timer(&test_timer, jiffies + 1*HZ);

// DECLARE_TASKLET(my_tasklet1, func1, 0);
// DECLARE_TASKLET(my_tasklet2, func2, 0);