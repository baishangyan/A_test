// #include "../include/kernel_list.h"

// 以下是mmap_driver.c的源代码

// include <linux/module.h>  
// #include <linux/init.h>  
  
// #include <linux/types.h> //定义dev_t类型  
// #include <linux/cdev.h> //定义struct cdev结构体及相关操作  
// #include <linux/slab.h> //定义kmalloc接口  
// #include <asm/io.h>//定义virt_to_phys接口  
// #include <linux/mm.h>//remap_pfn_range  
// #include <linux/fs.h>  
  
// #define MAJOR_NUM 990  
// #define MM_SIZE 4096  
  
// static char driver_name[] = "mmap_driver1";//驱动模块名字  
// static int dev_major = MAJOR_NUM;  
// static int dev_minor = 0;  
// char *buf = NULL;  
// struct cdev *cdev = NULL;  
  
// static int device_open(struct inode *inode, struct file *file)  
// {  
//     printk(KERN_ALERT"device open\n");  
//     buf = (char *)kmalloc(MM_SIZE, GFP_KERNEL);//内核申请内存只能按页申请，申请该内存以便后面把它当作虚拟设备  
//     return 0;  
// }  
  
// static int device_close(struct inode *indoe, struct file *file)  
// {  
//     printk("device close\n");  
//     if(buf)  
//     {  
//         kfree(buf);  
//     }  
//     return 0;  
// }  
  
// static int device_mmap(struct file *file, struct vm_area_struct *vma)  
// {  
//     vma->vm_flags |= VM_IO;//表示对设备IO空间的映射  
//     vma->vm_flags |= VM_RESERVED;//标志该内存区不能被换出，在设备驱动中虚拟页和物理页的关系应该是长期的，应该保留起来，不能随便被别的虚拟页换出  
//     if(remap_pfn_range(vma,//虚拟内存区域，即设备地址将要映射到这里  
//                        vma->vm_start,//虚拟空间的起始地址  
//                        virt_to_phys(buf)>>PAGE_SHIFT,//与物理内存对应的页帧号，物理地址右移12位  
//                        vma->vm_end - vma->vm_start,//映射区域大小，一般是页大小的整数倍  
//                        vma->vm_page_prot))//保护属性，  
//     {  
//         return -EAGAIN;  
//     }  
//     return 0;  
// }  
  
// static struct file_operations device_fops =  
// {  
//     .owner = THIS_MODULE,  
//     .open  = device_open,  
//     .release = device_close,  
//     .mmap = device_mmap,  
// };  
  
// static int __init char_device_init( void )  
// {  
//     int result;  
//     dev_t dev;//高12位表示主设备号，低20位表示次设备号  
//     printk(KERN_ALERT"module init2323\n");  
//     printk("dev=%d", dev);  
//     dev = MKDEV(dev_major, dev_minor);  
//     cdev = cdev_alloc();//为字符设备cdev分配空间  
//     printk(KERN_ALERT"module init\n");  
//     if(dev_major)  
//     {  
//         result = register_chrdev_region(dev, 1, driver_name);//静态分配设备号  
//         printk("result = %d\n", result);  
//     }  
//     else  
//     {  
//         result = alloc_chrdev_region(&dev, 0, 1, driver_name);//动态分配设备号  
//         dev_major = MAJOR(dev);  
//     }  
      
//     if(result < 0)  
//     {  
//         printk(KERN_WARNING"Cant't get major %d\n", dev_major);  
//         return result;  
//     }  
      
      
//     cdev_init(cdev, &device_fops);//初始化字符设备cdev  
//     cdev->ops = &device_fops;  
//     cdev->owner = THIS_MODULE;  
      
//     result = cdev_add(cdev, dev, 1);//向内核注册字符设备  
//     printk("dffd = %d\n", result);  
//     return 0;  
// }  
  
// static void __exit char_device_exit( void )  
// {  
//     printk(KERN_ALERT"module exit\n");  
//     cdev_del(cdev);  
//     unregister_chrdev_region(MKDEV(dev_major, dev_minor), 1);  
// }  
  
// module_init(char_device_init);//模块加载  
// module_exit(char_device_exit);//模块退出  
  
// MODULE_LICENSE("GPL");  
// MODULE_AUTHOR("ChenShengfa");  
 

// 下面是测试代码test_mmap.c

// [cpp] view plain copy

// #include <stdio.h>  
// #include <fcntl.h>  
// #include <sys/mman.h>  
// #include <stdlib.h>  
// #include <string.h>  
  
// int main( void )  
// {  
//     int fd;  
//     char *buffer;  
//     char *mapBuf;  
//     fd = open("/dev/mmap_driver", O_RDWR);//打开设备文件，内核就能获取设备文件的索引节点，填充inode结构  
//     if(fd<0)  
//     {  
//         printf("open device is error,fd = %d\n",fd);  
//         return -1;  
//     }  
//     /*测试一：查看内存映射段*/  
//     printf("before mmap\n");  
//     sleep(15);//睡眠15秒，查看映射前的内存图cat /proc/pid/maps  
//     buffer = (char *)malloc(1024);  
//     memset(buffer, 0, 1024);  
//     mapBuf = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);//内存映射，会调用驱动的mmap函数  
//     printf("after mmap\n");  
//     sleep(15);//睡眠15秒，在命令行查看映射后的内存图，如果多出了映射段，说明映射成功  
      
//     /*测试二：往映射段读写数据，看是否成功*/  
//     strcpy(mapBuf, "Driver Test");//向映射段写数据  
//     memset(buffer, 0, 1024);  
//     strcpy(buffer, mapBuf);//从映射段读取数据  
//     printf("buf = %s\n", buffer);//如果读取出来的数据和写入的数据一致，说明映射段的确成功了  
      
      
//     munmap(mapBuf, 1024);//去除映射  
//     free(buffer);  
//     close(fd);//关闭文件，最终调用驱动的close  
//     return 0;  
// }  