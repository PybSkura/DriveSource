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



static ssize_t led_write(struct file *filp, const char _user *buf,
                         size_t count, loff_t *ppos)
{
    char val;//在驱动程序定义的，所以实在内核空间

    /* copy_form_user : 从app得到数据*/
    copy_from_user(&val, buf, 1);

    /* 根据数据，设置 gpio register ： 输出1/0*/
    if(val){

        /* 设置gpio 令led亮起*/
    }
    else{

        /* 设置gpio 令led熄灭*/
    }

    return 1;//表示写了一个数据

}

static ssize_t led_open(struct file *filp, const char _user *buf,
                         size_t count, loff_t *ppos)
{
    /* 使能gpio
     * 配置某个引脚为gpio模式
     * 配置 gpio为输出模式
     * 
     */

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

    /* 通过下面两个函数，方便系统创建设备节点，会创建名为/dev/myled 的设备节点*/
    led_class = class_create(THIS_MODULE, "myled");
    device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"),
    return 0;
}

/* 出口函数*/
static int _init led_exit(void){

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
