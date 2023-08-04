#include<linux/module.h>

#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/miscdevice.h>
#include<linux/kernel.h>
#include<linux/major.h>
#include<linux/mutex.h>
#include<linux/proc_fs.h>
#include<linux/seq_file.h>
#include<linux/stat.h>
#include<linux/device.h>
#include<linux/tty.h>
#include<linux/kmod.h>
#include<linux/gfp.h>

static int major;
static struct  class *led_class;

/* registers 寄存器地址*/
// GPIO5 地址： 0x02290000 + 0x14
// GPIO5_GDIR 地址： 0x020AC004 设置GPIO方向寄存器
// GPIO5_DR 地址： 0x020AC000 设置GPIO高低寄存器
static volatile unsigned int *GPIO5;//用volatile是因为访问寄存器不能让编译器优化，要指定变量不要优化
static volatile unsigned int *GPIO5_GDIR;
static volatile unsigned int *GPIO5_DR;


static ssize_t led_write(struct file *filp, const char _user *buf,
                         size_t count, loff_t *ppos)
{
    char val;//在驱动程序定义的，所以实在内核空间
    int ret;
    /* copy_form_user : 从app得到数据*/
    
    ret = copy_from_user(&val, buf, 1);

    /* 根据数据，设置 gpio register ： 输出1/0*/
    if(val){

        /* 设置gpio 令led亮起*/
        *GPIO5_DR &= ~(1<<3);//gpio输出低电平
    }
    else{

        /* 设置gpio 令led熄灭*/
        *GPIO5_DR |= (1<<3);//gpio输出高电平
    }

    return 1;//表示写了一个数据

}

static ssize_t led_open(struct file *filp, const char _user *buf,
                         size_t count, loff_t *ppos)
{
    /* 使能gpio   默认使能了已经
     * 配置某个引脚为gpio模式
     * 配置 gpio为输出模式
     * 
     */

    /* 配置引脚为gpio模式 */
    //其他驱动程序可能修改了该寄存器，因而要先清除寄存器
    *GPIO5 &= ~0xf;//清除寄存器
    *GPIO5 |= 0X05;//配置模式 将寄存器后三位改成101，要先转成16进制，然后或上

    /* 配置gpio为输出模式*/
    *GPIO5_GDIR |= (1<<3);//将bit3设置为1
}

/* file_operation 结构体*/
static struct file_operations led_drv = {
    .owner  =   THIS_MODULE,
    .open   =   led_open,
    .read   =   hello_drv_read,
    .write  =   led_write,
    .release=   hello_drv_close,
};

/* 入口函数*/
static int _init led_init(void){

    printf("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    /* 注册file_opration 结构体*/
    major = register_chrdev(0,"led",&led_fops);

    /* ioremap 驱动程序不能直接去访问寄存器，而是要给寄存器映射一个虚拟地址，访问这个虚拟地址*/

    GPIO5 = ioremap(0x02290000 + 0x14, 4);//参数1：物理地址 参数2：映射多大空间
    GPIO5_GDIR = ioremap(0x020AC004, 4);
    GPIO5_DR = ioremap(0x020AC000, 4);

    /* 通过下面两个函数，方便系统创建设备节点，会创建名为/dev/myled 的设备节点*/
    led_class = class_create(THIS_MODULE, "myled");
    device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"),
    return 0;
}

/* 出口函数*/
static int _init led_exit(void){

    /* 取消地址映射*/
    iounmap(GPIO5);
    iounmap(GPIO5_GDIR);
    iounmap(GPIO5_DR);
    /* 销毁设备节点*/
    device_destroy(led_class, MKDEV(major, 0));
    class_destroy(led_class);
    /* 销毁file_operation 结构体*/
    unregister_chrdev(major, "led");


    return 0;
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
