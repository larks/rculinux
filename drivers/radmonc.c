#include <linux/module.h>	 // Module macros
#include <linux/version.h>
#include <linux/kernel.h>    // 
#include <linux/fs.h>        // File structures and methods
#include <linux/types.h>     // Datatypes
#include <linux/kdev_t.h>    // Major and minor macros
#include <linux/device.h>    // 
#include <linux/cdev.h>      // 
#include <asm/uaccess.h>     // user space memory access
#include <asm/io.h>          // ioremap etc.
#include <linux/moduleparam.h>
//#include <linux/arch/arm/mach-m2s/gpio.h>
#include <linux/gpio.h>

// Fixed major number
static uint radmonc_majorID = 253;

// pin names

//static struct gpio jtag_radmon[] = {
//		{2, GPIOF_OUT_INIT_LOW, "Radmon TCK"}, /*TCK going to Radmon*/
//		{3, GPIOF_OUT_INIT_LOW, "Radmon TDO"}, /*TDO going to Radmon TDI*/
//		{4, GPIOF_OUT_INIT_LOW, "Radmon TMS"}, /*TMS*/
//		{5, GPIOF_OUT_INIT_HIGH, "Radmon TRST"}, /*nTRST going to Radmon*/
//		{6, GPIOF_IN_INIT_LOW, "Radmon TDI"} /*TDI to Radmon TDO*/
//};

/*
* Here comes file operation functions:
*/
// File open
static int radmonjtag_open(struct inode* inode, struct file* file_p)
{
	int i;
	printk("hello cat!\n");
	i = gpio_get_value(6);
	printk("gpio_get_value=0x%X\n", i);
	return 0;
}
// File close 
static int radmonjtag_close(struct inode* inode, struct file* file_p)
{
	return 0;
}
// File read
static ssize_t radmonjtag_read(struct file* file_p, 
                         char __user* buffer, 
                         size_t count, 
                         loff_t* f_pos)
{
// size_t read(int fildes, void *buf, size_t nbytes);
// read(com_port, &data,1), ex where data=0x50
// file_p = com_port, buffer = data, count=1	
	
	int ret;
	ret = gpio_get_value(6);
	if(copy_to_user(buffer, &ret, 1) != 0)
		return -EFAULT;
	else
	//printk("ret=%d\n", ret);
	return ret;
}
// File write
static ssize_t radmonjtag_write(struct file *file_p, 
                          const char __user *buffer, 
                          size_t length, 
                          loff_t* f_pos)
{
//	ssize_t write(int fildes, const void *buf, size_t nbytes);
// ex: write(com_port, &data, 1); where data=0x70;	
    int ret; int gpio;
    copy_from_user(&);
//	printk("write not implemented\n");
	return 0;	
}
// Define our custom file operation structure
static struct file_operations radmonc_fops =
{
	.owner = THIS_MODULE,
	.open = radmonjtag_open,
	.release = radmonjtag_close,
	.read = radmonjtag_read,
	.write = radmonjtag_write
};

/*
* Module operation functions
*/
static int __init radmonjtagmodule_init(void)
{
	int iResult = 0; // holding result of operations
	iResult = register_chrdev(radmonc_majorID, "radmonc", &radmonc_fops);
	if (iResult < 0) {
		printk(KERN_INFO "radmonc: can't register driver\n");
		//mrlogmessage(LOG_ERROR, KERN_ERR "module init: can't register driver");
	}
	else{
		iResult = gpio_request(6, "TDI");
		printk(KERN_INFO "radmonc: module registered, iResult=%d\n", iResult);
		iResult = gpio_direction_input(6);
		printk("radmonc: gpio_direction, iResult=%d\n", iResult);
	}			
	return iResult;
}

static void __exit radmonjtagmodule_exit(void)
{
	int iResult;
	//int iResult;
    unregister_chrdev(radmonc_majorID, "rcuc");
//    for (i=0; i<5; i++){
		gpio_free(6);
		//if(iResult<0){printk(KERN_WARNING "radmonc: unable to request GPIO_%d\n", i);}
		//else {printk(KERN_INFO "freed GPIO: i=%d, iResult=%d\n", i, iResult);}
        printk("Freed GPIO, iResult=%d\n",iResult);
//	}

	printk("Bye bye kernel\n"); // printed when succesfully closed
}

module_init(radmonjtagmodule_init);
module_exit(radmonjtagmodule_exit);

MODULE_AUTHOR("Lars Bratrud");
MODULE_DESCRIPTION("Radmon JTAG driver");
MODULE_LICENSE("GPL");
