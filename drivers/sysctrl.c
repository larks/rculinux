/*
 * sysctrl.c - The simplest loadable kernel module.
 * Intended as a template for development of more
 * meaningful kernel modules.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "query_ioctl.h"
#include "mss_sys_services/mss_comblk.h"
#include "mss_sys_services/mss_sys_services.h"

/*
 * Driver verbosity level: 0->silent; >0->verbose
 */
static int sysctrl_debug = 0;

/*
 * User can change verbosity of the driver
 */
module_param(sysctrl_debug, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(sysctrl_debug, "sysctrl driver verbosity level");

/*
 * Service to print debug messages
 */
#define d_printk(level, fmt, args...)				\
	if (sysctrl_debug >= level) printk(KERN_INFO "%s: " fmt,	\
					__func__, ## args)

/*
 * Device major number
 */
static uint sysctrl_major = 169;

/*
 * User can change the major number
 */
module_param(sysctrl_major, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(sysctrl_major, "sysctrl driver major number");

/*
 * Device name
 */
static char * sysctrl_name = "sysctrl";

/*
 * Device access lock. Only one process can access the driver at a time
 */
static int sysctrl_lock = 0;

/*
 * Device "data"
 */
static char sysctrl_str[] = "This is the simplest loadable kernel module\n";
static char *sysctrl_end;

/*
 * Device open
 */
static int sysctrl_open(struct inode *inode, struct file *file)
{
	int ret = 0;

	/*
	 * One process at a time
	 */
	if (sysctrl_lock ++ > 0) {
		ret = -EBUSY;
		goto Done;
	}

	/*
 	 * Increment the module use counter
 	 */
	try_module_get(THIS_MODULE);

	/*
 	 * Do open-time calculations
 	 */
	sysctrl_end = sysctrl_str + strlen(sysctrl_str);

Done:
	d_printk(2, "lock=%d\n", sysctrl_lock);
	return ret;
}

/*
 * Device close
 */
static int sysctrl_release(struct inode *inode, struct file *file)
{
	/*
 	 * Release device
 	 */
	sysctrl_lock = 0;

	/*
 	 * Decrement module use counter
 	 */
	module_put(THIS_MODULE);

	d_printk(2, "lock=%d\n", sysctrl_lock);
	return 0;
}

/* 
 * Device read
 */
static ssize_t sysctrl_read(struct file *filp, char *buffer,
			 size_t length, loff_t * offset)
{
	char * addr;
	unsigned int len = 0;
	int ret = 0;

	/*
 	 * Check that the user has supplied a valid buffer
 	 */
	if (! access_ok(0, buffer, length)) {
		ret = -EINVAL;
		goto Done;
	}

	/*
 	 * Get access to the device "data"
 	 */
	addr = sysctrl_str + *offset;

	/*
	 * Check for an EOF condition.
	 */
	if (addr >= sysctrl_end) {
		ret = 0;
		goto Done;
	}

	/*
 	 * Read in the required or remaining number of bytes into
 	 * the user buffer
 	 */
	len = addr + length < sysctrl_end ? length : sysctrl_end - addr;
	strncpy(buffer, addr, len);
	*offset += len;
	ret = len;

Done:
	d_printk(3, "length=%d,len=%d,ret=%d\n", length, len, ret);
	return ret;
}

/* 
 * Device write
 */
static ssize_t sysctrl_write(struct file *filp, const char *buffer,
			  size_t length, loff_t * offset)
{
	d_printk(3, "length=%d\n", length);
	return -EIO;
}

/* IOCTL */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int sysctrl_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long sysctrl_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	//query_arg_t query;
	switch(cmd)
	{
		case READ_IDCODE:
			d_printk(1, "Read ID code...\n");
			break;
		//case PROGRAM:
		//case VERIFY:
		default:
			return -EINVAL;
	
	}
	return 0;
}

/*
 * Device operations
 */
static struct file_operations sysctrl_fops = {
	.read = sysctrl_read,
	.write = sysctrl_write,
	.open = sysctrl_open,
	.release = sysctrl_release,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = sysctrl_ioctl
#else
	unlocked_ioctl = sysctrl_ioctl
#endif
};

static int __init sysctrl_init_module(void)
{
	int ret = 0;

	/*
 	 * check that the user has supplied a correct major number
 	 */
	if (sysctrl_major == 0) {
		printk(KERN_ALERT "%s: sysctrl_major can't be 0\n", __func__);
		ret = -EINVAL;
		goto Done;
	}

	/*
 	 * Register device
 	 */
	ret = register_chrdev(sysctrl_major, sysctrl_name, &sysctrl_fops);
	if (ret < 0) {
		printk(KERN_ALERT "%s: registering device %s with major %d "
				  "failed with %d\n",
		       __func__, sysctrl_name, sysctrl_major, ret);
		goto Done;
	}
	
Done:
	d_printk(1, "name=%s,major=%d\n", sysctrl_name, sysctrl_major);

	return ret;
}
static void __exit sysctrl_cleanup_module(void)
{
	/*
	 * Unregister device
	 */
	unregister_chrdev(sysctrl_major, sysctrl_name);

	d_printk(1, "%s\n", "clean-up successful");
}

module_init(sysctrl_init_module);
module_exit(sysctrl_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars Bratrud, lars.bratrud@cern.ch");
MODULE_DESCRIPTION("sysctrl device driver");