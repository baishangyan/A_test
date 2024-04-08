#include "../include/driver_module.h"


void driver_main(void)
{
    int fd;
    int val = 1;
    fd = open("/dev/mes", O_RDWR, 0);
    if(fd < 0){
        printf("can't open!n");
    }
    write(fd, &val, 4);
}
//驱动文件hello.c
// #include <linux/module.h>
// #include <linux/kernel.h>
// #include <linux/fs.h>
// #include <linux/init.h>
// #include <linux/delay.h>
// #define HELLO_MAJOR 231
// #define DEVICE_NAME "HelloModule"
// static int hello_open(struct inode *inode, struct file *file){
//     printk(KERN_EMERG "hello open.n");
//     return 0;
// }
// static int hello_write(struct file *file, const char __user * buf, size_t count, loff_t *ppos){
//     printk(KERN_EMERG "hello write.n");
//     return 0;
// }
// static struct file_operations hello_flops = {
//     .owner = THIS_MODULE,
//     .open = hello_open,
//     .write = hello_write,
// };
// static int __init hello_init(void){
//     int ret;
//     ret = register_chrdev(HELLO_MAJOR,DEVICE_NAME, &hello_flops);
//     if (ret < 0) {
//         printk(KERN_EMERG DEVICE_NAME " can't register major number.n");
//         return ret;
//     }
//     printk(KERN_EMERG DEVICE_NAME " initialized.n");
//     return 0;
// }
// static void __exit hello_exit(void){
//     unregister_chrdev(HELLO_MAJOR, DEVICE_NAME);
//     printk(KERN_EMERG DEVICE_NAME " removed.n");
// }
// module_init(hello_init);
// module_exit(hello_exit);
// MODULE_LICENSE("GPL");

// printk打印分8个等级，等级高的被打印到控制台上，而等级低的却输出到日志文件中。

/*
二、驱动实例测试
　　测试的方法整体来说就是，编译驱动和上层测试应用；加载驱动，通过上层应用调用驱动；最后，卸载驱动。

1、编译驱动
#make
　　make命令，直接调用Makefile编译hello.c，最后会生成“hellomodule.ko”。

2、编译上层应用
#gcc hellotest.c -o hellotest
　　通过这条命令，就能编译出一个上层应用hellotest。

3、加载驱动
#insmod hellomodule.ko
　　insmod加载驱动的时候，会调用函数hello_init()，打印的调试信息如下。
　　此外，在"/proc/devices"中可以看到已经加载的模块。

4、创建节点
　　虽然已经加载了驱动hellomodule.ko，而且在/proc/devices文件中也看到了已经加载的模块HelloModule，
    但是这个模块仍然不能被使用，因为在设备目录/dev目录下还没有它对应的设备文件。所以，需要创建一个设备节点。

#mknod /dev/hellodev c 231 0
　　在/proc/devices中看到HelloModule模块的主设备号为231，创建节点的时候就是将设备文件/dev/hellodev与主设备号建立连接。
    这样在应用程序操作文件/dev/hellodev的时候，就会定位到模块HelloModule。

/proc/devices 与 /dev的区别
/proc/devices中的设备是驱动程序生成的，它可产生一个major供mknod作为参数。这个文件中的内容显示的是当前挂载在系统的模块。
当加载驱动HelloModule的时候，并没有生成一个对应的设备文件来对这个设备进行抽象封装，以供上层应用访问。

/dev下的设备是通过mknod加上去的，用户通过此设备名来访问驱动。我以为可以将/dev下的文件看做是硬件模块的一个抽象封装，
Linux下所有的设备都以文件的形式进行封装。

5、上层应用调用驱动
#./hellotest
　　hellotest应用程序先打开文件“/dev/hellodev”，然后向此文件中写入一个变量val。
    期间会调用底层驱动中的hello_open和hello_write函数.

6、卸载驱动
#rmmod hellomodule
　　rmmod卸载驱动的时候，会调用函数hello_exit()，打印的调试信息如下。

总结一个模块的操作流程：
　　（1）通过insmod命令注册module
　　（2）通过mknod命令在/dev目录下建立一个设备文件"xxx"，并通过主设备号与module建立连接
　　（3）应用程序层通过设备文件/dev/xxx对底层module进行操作

三、驱动模板
　　从宏观上把握了驱动程序的框架，然后再从细节上完善驱动的功能，这是开发驱动程序的一般步骤。
驱动模板必备要素有头文件、初始化函数、退出函数、版权信息，常用的扩展要素是增加一些功能函数完善底层驱动的功能。
init.h 　　　　定义了驱动的初始化和退出相关的函数
kernel.h 　　 定义了经常用到的函数原型及宏定义
module.h 　　定义了内核模块相关的函数、变量及宏
2、初始化函数
static int __init hello_init(void)

当加载驱动到内核的时候，这个初始化函数就会被自动执行。
初始化函数顾名思义是用来初始化模块的，常用的功能是通过register_chrdev来注册函数。
内核分配了一块内存（数组）专门用来存放字符设备的函数集，register_chrdev函数会在这个数组的HELLO_MAJOR位置将
hello_flops中的内容进行填充，也就是将HelloModule的功能函数地址注册到设备管理内存集中。

3、退出函数
static void __exit hello_exit(void)

当卸载驱动的时候，退出函数便会自动执行，完成一些列清除工作。
在加载驱动的时候，我们向设备管理内存集中注册了该模块的相关功能函数。
当卸载驱动的时候，就有必要将这个模块占用的内存空间清空。这样当其他的设备注册的时候便有更多的空间可以选择。

4、版权信息
MODULE_LICENSE("GPL");
Linux内核是按照GPL发布的，同样Linux的驱动程序也要提供版权信息，否则当加载到内核中系统会给出警告信息。

5、功能函数
static int hello_open(struct inode *inode, struct file *file)
static int hello_write(struct file *file, const char __user * buf, size_t count, loff_t *ppos)
static struct file_operations hello_flops

功能函数虽然不是一个驱动模板所必须的，但是一个有实际意义的驱动程序一定包含功能函数。
功能函数实际上定义了这个驱动程序为用户提供了哪些功能，也就是用户可以对一个硬件设备可以进行哪些操作。
常见的功能函数有xxx_open()、xxx_write()、xxx_read()、xxx_ioctl()、xxx_llseek()等。
当上层应用调用open()、write()、read()、ioctl()、llseek()等这些函数的时候，经过层层调用最后到达底层，
调用相应的功能函数。结构体file_operations中的成员定义了很多函数，实际应用可以只对其部分成员赋值，其定义如下。
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
    ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
    int (*readdir) (struct file *, void *, filldir_t);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, struct dentry *, int datasync);
    int (*aio_fsync) (struct kiocb *, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);
    ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
    unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
    int (*check_flags)(int);
    int (*flock) (struct file *, int, struct file_lock *);
    ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
    ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
    int (*setlease)(struct file *, long, struct file_lock **);
};

四、从上层应用到底层驱动的执行过程

1、Linux系统的分层结构
    Linux系统的分层结构为：应用层 ----> 库 ----> 内核 ----> 驱动程序 ----> 硬件设备。

2、从上层应用到底层驱动的执行过程
　　以“open("/dev/hellodev", O_RDWR)”函数的执行过程为例来说明。

（1）应用程序使用库提供的open函数打开代表hellodev的设备文件。
（2）库根据open函数传入的参数执行swi指令，这条指令会引起CPU异常，从而进入内核。
（3）内核的异常处理函数根据这些参数找到相应的驱动程序。
（4）执行相应的驱动程序。
（5）返回一个文件句柄给库，进而返回给应用程序。

3、驱动程序的执行特点
　　与应用程序不同，驱动程序从不主动运行，它是被动的：根据应用程序的要求进行初始化，根据应用程序的要求进行读写。
    驱动程序加载进内核，只是告诉内核“我在这里，我能做这些工作”，至于这些工作何时开始，则取决于应用程序。
　　驱动程序运行于“内核空间”，它是系统“信任”的一部分，驱动程序的错误有可能导致整个系统的崩溃。
*/

/*
【注意】：kzalloc后的空间是不需要再执行memset的，因为它本身就包含了这个操作。而memset一般作用在已经存在的空间上。

由此分析：

cdev_alloc函数针对于需要空间申请的操作，而cdev_init针对于不需要空间申请的操作；因此如果你定义的是一个指针，
那么只需要使用cdev_alloc函数并在其后做一个ops的赋值操作就可以了；如果你定义的是一个结构体而非指针，
那么只需要使用cdev_init函数就可以了。

看到有些代码在定义一个指针后使用了cdev_alloc函数，紧接着又使用了cdev_init函数，这个过程不会出现错误，
但只是做了一些重复的无用工作，其实完全可以不需要的。

================================================================================================
2、注册/注销驱动必须使用倒影式

内核中很多函数中包含了很多个操作，这些操作每一步都有可能出错，而且出错后后面的步骤就没有进行下去的必要性了。

解决方案：倒影式机制，（先注册，后注销）

3、手动创建 ——>自动创建  字符设备驱动

手动创建：老接口分析

register_chrdev
__register_chrdev
__register_chrdev_region
cdev_alloc
cdev_add

手动创建过程：

装载：先insmod装载设备，再使用mknod创建设备文件【mknod /dev/test c 250 0】

卸载：先rmmod卸载设备，再删除mknod创建的设备文件



自动创建：新接口分析

register_chrdev_region
__register_chrdev_region

alloc_chrdev_region
__register_chrdev_region

自动创建过程：

装载：直接insmod，装载设备的同时创建文件

卸载：直接rmmod，卸载设备的同时删除创建的文件 

udev（嵌入式中用的是mdev）
(1)什么是udev？应用层的一个应用程序
(2)内核驱动和应用层udev之间有一套信息传输机制（netlink协议）
(3)应用层启用udev，内核驱动中使用相应接口
(4)驱动注册和注销时信息会被传给udev，由udev在应用层进行设备文件的创建和删除

内核驱动设备类相关函数
(1)class_create//创建类
(2)device_create//创建设备

(2)device_destroy//销毁设备
(1)calss_destroy//销毁类

4、相关函数 & 结构体分析

注册字符设备驱动新接口：

0——驱动向内核注册自己的函数register_chrdev()
static inline int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops)

unsigned int major,主设备号
const char *name,设备名称 
const struct file_operations *fops： 
向函数内部传参（将自己写的file_operations传给register_chrdev()函数，在该函数内部完成自己编写的file_operations结构体的注册）
，因为file_operations是输入型参数，故前面要加const修饰

常见错误定义文件目录：linux/asm-generic/errno-base.h
头文件包含：#include <linux/errno.h>
cat /proc/devices 查看当前设备

1——新接口与老接口
(1)老接口：register_chrdev:绑定file_operations结构体和主次设备号（可自动分配也可自己设定）
(2)新接口：register_chrdev_region/alloc_chrdev_region + cdev
register_chrdev_region：指定设备号，让内核直接分配
alloc_chrdev_region + cdev：自动分配设备号
cdev：字符驱动设备的注册


2——cdev结构体介绍：
包含于 kernel/linux/cdev.h文件中
struct cdev {
struct kobject kobj;
struct module *owner;//用来将其与我们的模块挂钩
const struct file_operations *ops;//file_operations 结构体变量
struct list_head list;
dev_t dev;  //设备号 = 主设备号 + 次设备号 dev_t：代表设备号
unsigned int count;//计数，（记录read或write的次数）
};
3——相关函数：cdev_alloc、cdev_init、cdev_add、cdev_del
cdev_alloc ：给cdev结构体申请内存空间
cdev_init  ：初始化
cdev_add  ：添加一个设备驱动
cdev_del  : 注销一个设备驱动

4——设备号
(1)主设备号和次设备号
(2)dev_t类型
(3)MKDEV、MAJOR、MINOR三个宏
MKDEV ：将主设备号和次设备号构成设备号
MAJOR ：提取主设备号
MINOR ：提取次设备号
5——编程实践
(1)使用register_chrdev_region + cdev_init + cdev_add进行字符设备驱动注册,分两步：
//第一步：分配主次设备号
mydev = MKDEV(MYMAJOR, 0); //获取设备号
int register_chrdev_region(dev_t from, unsigned count, const char *name)//绑定file_operations结构体和主次设备号
{
return 0;
}
dev_t from  :起始设备号 = 主设备号 + 次设备号
unsigned count:次设备号数量
const char *name：设备名称
//第二步：注册字符设备驱动
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
struct cdev *cdev//字符驱动设备，（函数指针）
const struct file_operations *fops//file_operations结构体（函数指针）


int cdev_add(struct cdev *p, dev_t dev, unsigned count)
struct cdev *p//字符驱动设备，（函数指针）
dev_t dev  //获取的设备号


6——注销设备驱动
// 第一步真正注销字符设备驱动用cdev_del
void cdev_del(struct cdev *p); //struct cdev *p字符驱动设备，（函数指针）
// 第二步去注销申请的主次设备号
unregister_chrdev_region(USB_DEVICE_DEV, USB_DEVICE_MAX);
USB_DEVICE_DEV//获取的设备号 
USB_DEVICE_MAX//次设备号数量


7——//自动分配设备号alloc_chrdev_region + cdev
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)
dev_t *dev //输出型参数——也就是系统自动分配的主设备号
unsigned baseminor //次设备号的基准
unsigned count //次设备号数量
const char *name //

概述
       Unix类系统将设备也看作是文件，通过操作文件的方式操作硬件。而操作文件的方式无非就是open、read、write、close等，
       将这些通用的文件操作函数抽象出来，就是file_operation结构体。（实际内核定义如下，可以看见各种会用到的函数里面都有）

struct file_operations {
    struct module *owner;
    ...
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    ...
    int (*open) (struct inode *, struct file *);
    ...
 
};
      那么显然，要想编写出实际有用的代码，我们起码要实现对应的函数，这样当你在应用程序中调用open，就会实际调用设备驱动的open。
      其重要性可见一斑。【思考一：内核是如何将app里的操作函数和驱动里的操作函数联系上的呢？】先通过led实验了解如何简单实用，
      和驱动的简单框架。

 

实验
目标：点亮开发板上的led灯

一、包含头文件，写出入口和出口函数，添加必要协议和修饰

头文件，协议和修饰就略了，要是这不知道就去看前一篇吧。《（1） 设备驱动的最基本框架》

1. 入口函数的实现

unsigned int major;
 
static int __init leds_drv_init(void) // 入口函数 
{
    unsigned int minor = 0;
 
    gpio_va = ioremap(0x56000000, 0x100000);
    if (!gpio_va) {
        return -EIO;
    }
 
    major = register_chrdev(0, "leds_dev", &jz2440_leds_fops); //注册 告诉内核
    if (major < 0)
    {
        printk("leds_dev can't register major number\n");
        return major;
    }
 
    leds_class = class_create(THIS_MODULE, "leds_class"); //创建一个类
 
    leds_dev[0] = device_create(leds_class, NULL, MKDEV(major, 0), NULL, "leds"); //在类下创建设备节点"leds"
    for (minor = 1; minor < 4; minor++)
    {
        leds_dev[minor] = device_create(leds_class, NULL, MKDEV(major, minor), NULL, "led%d", minor);     //创建设备节点"ledx" 次设备号不同
    }
 
    printk("leds_dev initialized\n");
    return 0;
}
2.主设备号的概念：linux中使用ls -l 查看某个设备的具体信息时，可以看见主设备号和次设备号，内核通过主设备来选择对应的设备和驱动，
次设备号则是纯软件的概念，用于用同一个设备驱动使用几个不同的外设，比如一个led_drv驱动led0,led1,led2... 【
思考二；内核怎样创建设备，主设备号具体应用是什么呢？】

/* 注册字符设备
* 参数为主设备号、设备名字、file_operations结构；
* 这样，主设备号就和具体的file_operations结构联系起来了，
* 操作主设备为LED_MAJOR的设备文件时，就会调用s3c24xx_leds_fops中的相关成员函数
* LED_MAJOR可以设为0，表示由内核自动分配主设备号

 
major = register_chrdev(0, "leds_dev", &jz2440_leds_fops); //注册 告诉内核
内核提供了一个register_chrdev()函数用来注册设备（英文翻译一下register char device）,

        第一个参数：0，写0表示让内核自动分配主设备号，并返回给major；

        第二个参数：“leds_dev”, 设备名；

        第三个参数；&jz2440_leds_fops，指向一个真实自定义file_operation结构体；

static const struct file_operations jz2440_leds_fops = {
    .owner = THIS_MODULE,
    .open = leds_drv_open,
    .write = leds_drv_write,
};
显然正确的实现这个结构体便是重点。

3. 注册设备类，在类下注册具体设备节点

【思考三：为什么要创建类呢？】

leds_class = class_create(THIS_MODULE, "leds_class"); //创建一个类
【思考四：创建设备节点的具体过程？】

leds_dev[0] = device_create(leds_class, NULL, MKDEV(major, 0), NULL, "leds"); //创建设备节点"leds"
for (minor = 1; minor < 4; minor++)
{
    leds_dev[minor] = device_create(leds_class, NULL, MKDEV(major, minor), NULL, "led%d", minor);    //创建设备节点"ledx" 次设备号不同
}
从这里也可以看出，主设备号是内核用的，次设备号是纯软的概念，给我们编程用的

4. 操作硬件所需要的具体物理地址映射，将物理地址映射为内核的虚拟地址，ioremap为内核提供的函数
 

static unsigned long gpio_va;
 
#define GPIO_OFT(x) ((x) - 0x56000000)      /* 具体物理地址有s3c2440芯片手册得来 
#define GPFCON (*(volatile unsigned long *)(gpio_va + GPIO_OFT(0x56000050)))
#define GPFDAT (*(volatile unsigned long *)(gpio_va + GPIO_OFT(0x56000054)))
 
gpio_va = ioremap(0x56000000, 0x100000);    /* 参数：物理起始地址，要映射的大小1M 
5.出口函数的实现

static void __exit leds_drv_exit(void)
 
{
 
    unsigned int minor;
 
    iounmap(gpio_va);
    unregister_chrdev(major, "leds_dev"); //卸载设备驱动，注意第二个参数要与注册时保持一致
    for (minor = 0; minor < 4; minor++)
 
    {
       device_unregister(leds_dev[minor]); //卸载类下的设备
    }
    class_destroy(leds_class);             //卸载类
 
}
看那些函数，就是调用了在入口函数中调用函数的相反函数，上面注册，下面注销，一一对应

 

二、file_operation结构体内函数的实现

那么长时间终于再次回到了我们的课题，file_operation

static const struct file_operations jz2440_leds_fops = {
    .owner = THIS_MODULE,
    .open = leds_drv_open,
    .write = leds_drv_write,
};
file_operations 可以用来实现很多功能，这里我们只要实现两个，open和write

static int leds_drv_open(struct inode *inode, struct file *file)
{
    /*配置GPF4，5，6
    GPFCON &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2))); //现将对应位清零
    GPFCON |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2))); // 01 输出
    return 0;
}
函数名自己取，需要传递的参数呢从file_operation结构体复制

有过裸板开发的应该很清楚，想点亮led的无非两步，

        a、配置对应GPIO的配置寄存器，使其为输出模式，

        b、再修改对应的数据寄存器，使其输出，就可以点亮led灯了。

显然这里的leds_drv_open()函数，就是配置寄存器

static ssize_t leds_drv_write(struct file *file, const char *buf, size_t count, loff_t *pos)
 
{
    unsigned int minor = MINOR(file->f_inode->i_rdev); //MINOR(inode->i_cdev); 得到次设备号
    char val;
 
    copy_from_user(&val, buf, count); //将数据从用户空间传到内核空间 反之copy_to_user
 
    switch (minor) /* 看，这就是次设备号的纯软的概念，和它的基本应用 
    {
        case 0: /* /dev/leds 
        {
            printk("/dev/leds: %d %d\n", minor, val);
            if(val==1)
            {
                GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
            }
            else
            {
                GPFDAT |= (1<<4) | (1<<5) | (1<<6);
            }
            break;
        }
 
        case 1: /* /dev/led1 
        {
            if(val==1)
            {
                GPFDAT &= ~(1<<4);
            }
            else
            {
                GPFDAT |= (1<<4);
            }
            break;
        }
        case 2: /* /dev/led2 
        {
            if(val==1)
            {
                GPFDAT &= ~(1<<5);
            }
            else
            {
                GPFDAT |= (1<<5);
            }
            break;
        }
        case 3: /* /dev/led3 
        {
            if(val==1)
            {
                GPFDAT &= ~(1<<6);
            }
            else
            {
                GPFDAT |= (1<<6);
            }
            break;
 
        }
        default: printk("/dev/leds: %d %d\ ?? n", minor, val);
    }
 
    return 0;
}
这样我们就实现了具体的file_operation结构体的具体功能函数，

major = register_chrdev(0, "leds_dev", &jz2440_leds_fops); //在这里传入，使之与具体的设备关联在一起
三、测试程序的实现

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
 
void print_usage(char *drv);
 
int main(int argc, char **argv)
{
    int fd;
    char* filename;
    char val = 1;
 
    if (argc != 3)
    {
        print_usage(argv[0]);
        return 1;
    }
 
    filename = argv[1];
 
    fd = open(filename, O_RDWR);
    if (fd < 0)
        printf("can't open dev nod !\n");
 
    if (strcmp(argv[2], "on") == 0)
    {
        val = 1;
    }
    else if (strcmp(argv[2], "off") == 0)
    {
        val = 0;
    }
    else
    {
        print_usage(argv[0]);
        return 1;
    }
 
    printf("%s : %d\r\n", argv[1], val);
    write(fd, &val, 1);
 
    return 0;
}
 
// 打印用法 
void print_usage(char *drv)
{
    printf("Usage : \n");
    printf("%s <dev> <on|off>\n", drv);
    printf("eg. \n");
    printf("%s /dev/leds on\n", drv);
    printf("%s /dev/leds off\n", drv);
    printf("%s /dev/led1 on\n", drv);
    printf("%s /dev/led1 off\n", drv);
    // <>表示内部的参数不可省略，|表示或 ？不把argv[0]打印出来避免一个越界错误
}
编译之后也放在板子上，安装模块，就可以测试了

*/

