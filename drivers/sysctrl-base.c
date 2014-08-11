/************************************************************************
**
**
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2014
** This file has been written by Lars Bratrud,
** Lars.Bratrud@cern.ch
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**
*************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /*get_user(), put_user()*/
#include <linux/types.h>

#include <linux/interrupt.h>
#include <linux/irq.h>

#include "query_ioctl.h"
#include "mss_sys_services/mss_comblk.h"
#include "mss_sys_services/mss_sys_services.h"

#define SUCCESS 0

/*
 * Driver verbosity level: 0->silent; >0->verbose
 */
static int sysctrl_debug = 1;
uint8_t status;
uint8_t serial_number[16] = {0};
uint32_t i;

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
	struct sample_dev *dev = hwinfo + MINOR(inode->i_rdev);
	request_irq(dev->irq, ComBlk_IRQHandler, 0, "sysctrl", dev);
	*/
	int ret_irq;
	ret_irq = request_irq(ComBlk_IRQn, ComBlk_IRQHandler, 0, "sysctrl", 0);
	if(ret_irq < 0){
		d_printk(1,"request_irq failed with %d", ret_irq);
		return -1;
	}

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
	free_irq(ComBlk_IRQn, 0);
	/*
 	 * Release device
 	 */
 	/*free_irq(ComBlk_IRQn, dev);*/
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
	/* First do some general setup of the System Controller and 
	 * Communication Block...
	*/
	//query_arg_t query;
	/* Remove this if the stuff in 'open' and 'release' works (it should damnit) */
	/*
	int ret_irq;
	ret_irq = request_irq(ComBlk_IRQn, ComBlk_IRQHandler, 0, "sysctrl", 0);
	if(ret_irq < 0){
		d_printk(1,"request_irq failed with %d", ret_irq);
		return -1;
	}
	
	else MSS_SYS_init(MSS_SYS_NO_EVENT_HANDLER);
	*/
	MSS_SYS_init(MSS_SYS_NO_EVENT_HANDLER);
	/* Then we check the command 
	 *
	*/
	switch(cmd)
	{
		case READ_IDCODE:
			d_printk(2, "Read serial number...\n");
			
			d_printk(2, "Passed init.\n");
			d_printk(2, "Before we get serial number, the variable is: %s", serial_number);
			status = MSS_SYS_get_serial_number(serial_number);
			d_printk(2, "Passed serial number get.\n");
			d_printk(2, "Status: %#x", status);
			if(MSS_SYS_SUCCESS == status){
				d_printk(1, "Got serial number: %#02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", serial_number[15], serial_number[14]
				, serial_number[13], serial_number[12], serial_number[11], serial_number[10]
				, serial_number[9],  serial_number[8],  serial_number[7],  serial_number[6]
				, serial_number[5],  serial_number[4],  serial_number[3],  serial_number[2]
				, serial_number[1],  serial_number[0]);
				break;
			}
			else if(MSS_SYS_MEM_ACCESS_ERROR == status){
				d_printk(1, "MSS_SYS_MEM_ACCESS_ERROR");
				break;
			}
			else if(MSS_SYS_UNEXPECTED_ERROR == status){
				d_printk(1, "MSS_SYS_UNEXPECTED_ERROR");
				break;
			}
			else d_printk(0, "Failed to get serial number\n");
            // testa MSS_SYS_init(void);
			break;
		case PROG_AUTHENTICATE:
			d_printk(1, "Authentification...");
			/*
			g_isp_operation_busy = 1;
			g_src_image_target_address = 0;
			MSS_SYS_start_isp(MSS_SYS_PROG_AUTHENTICATE, page_read_handler, isp_completion_handler);
			while(g_isp_operation_busy){ ; }
			if(!g_isp_operation_busy){
				//f_close(file);
				if(g_error_flag == MSS_SYS_SUCCESS){
					d_printk(1, "ISP Authentification completed successfully!");
				}
				else{
					d_printk(1, "ISP Authentification failed!");
				}
			}
			*/
			break;
		//case VERIFY:
		default:
			return -EINVAL;
	
	}
	/*free_irq(ComBlk_IRQn, 0);*/
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
MODULE_DESCRIPTION("System controller device driver");
