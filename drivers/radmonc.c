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

/*
#include "mrKernLogging.c"   // kernel debugging
*/
// Fixed major number
static uint radmonc_majorID = 253;

// Access control
static int driver_lock = 0;

/*
* Here comes file operation functions:
*/
// File open
static int radmonjtag_open(struct inode* inode, struct file* file_p)
{
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
	printk("read not implemented\n");
	return 0;
}
// File write
static ssize_t radmonjtag_write(struct file *file_p, 
                          const char __user *buffer, 
                          size_t length, 
                          loff_t* f_pos)
{
	printk("write not implemented\n");
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
	int i;
	int iResult = 0; // holding result of operations
	iResult = register_chrdev(rcuc_majorID, "radmonc", &radmonc_fops);
	if (iResult < 0) {
		printk(KERN_INFO "radmonc: can't register driver\n");
		//mrlogmessage(LOG_ERROR, KERN_ERR "module init: can't register driver");
	}
	else{
		printk(KERN_INFO "radmonc: module registerd\n");
		for (i=0; i<5; i++){
			iResult = gpio_request(i, "GPIO Jtag");
			if(iResult<0){printk(KERN_WARNING "radmonc: unable to request GPIO_%d\n", i);}
			else {printk(KERN_INFO "requested GPIO: i=%d, iResult=%d\n", i, iResult);}
		}
	}			
	return iResult;
}

static void __exit radmonjtagmodule_exit(void)
{
	int i;
	int iResult;
    unregister_chrdev(radmonc_majorID, "rcuc");
    for (i=0; i<5; i++){
		iResult = gpio_free(i);
		if(iResult<0){printk(KERN_WARNING "radmonc: unable to request GPIO_%d\n", i);}
		else {printk(KERN_INFO "freed GPIO: i=%d, iResult=%d\n", i, iResult);}
	}

	printk("Bye bye kernel\n"); // printed when succesfully closed
}

module_init(radmonjtagmodule_init);
module_exit(radmonjtagmodule_exit);

MODULE_AUTHOR("Lars Bratrud");
MODULE_DESCRIPTION("Radmon JTAG driver");
MODULE_LICENSE("GPL");
