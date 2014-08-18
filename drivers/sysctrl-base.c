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
#include <linux/errno.h>

#include <linux/interrupt.h>
#include <linux/irq.h>

#include "query_ioctl.h"
#include "mss_sys_services/mss_comblk.h"
#include "mss_sys_services/mss_sys_services.h"

#define SUCCESS 0u
#define BUF_LEN 512u

/* Driver verbosity level: 0->silent; >0->verbose */
static int sysctrl_debug = 0;
/* Misc. variables */
uint8_t status;
uint8_t serial_number[16] = {0};
uint32_t i = 0;
/* Device message */
static char Message[BUF_LEN];
static char *Message_Ptr;

/* DEBUGGING */
module_param(sysctrl_debug, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(sysctrl_debug, "sysctrl driver verbosity level");
/* Service to print debug messages */
#define d_printk(level, fmt, args...)				\
	if (sysctrl_debug >= level) printk(KERN_INFO "%s: " fmt,	\
					__func__, ## args)

/* Device major number */
static uint sysctrl_major = 169;

/* User can change the major number */
module_param(sysctrl_major, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(sysctrl_major, "sysctrl driver major number");

/* Device name */
static char * sysctrl_name = "sysctrl";

/* Device access lock. Only one process can access the driver at a time */
static int sysctrl_lock = 0;

/* 
** ISP related helper functions, handlers and variables
*/
volatile uint8_t g_isp_operation_busy;
static uint32_t g_error_flag;
uint8_t g_page_buffer[BUF_LEN];
uint8_t g_programming_mode = 0x0;
uint32_t g_length = 0;
const char *g_buffer;

void isp_completion_handler(uint32_t value)
{
	g_error_flag = value;
	g_isp_operation_busy = 0;
}

read_page_from_user(uint8_t * k_buffer, uint32_t length)
{
	uint32_t ibytes;
	if((ibytes = copy_from_user(&g_page_buffer, k_buffer, length)) != 0){
		return -EFAULT;
	}
	else
	return (length - ibytes);

}

uint32_t page_read_handler
(
    uint8_t const ** pp_next_page
)
{

//sysctrl_write(struct file *filp, const char *buffer, size_t length, loff_t * offset)

//copy_from_user( to, from, n );

//	uint8_t g_page_buffer[BUF_LEN];
    uint32_t length;
    //length = copy_from_user(&g_page_buffer, buffer, BUF_LEN);
    length = g_length;
    *pp_next_page = g_page_buffer;

    return length;
}

/*
** Device open
*/
static int sysctrl_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	int ret_irq;
	d_printk(3, "%p", file);
	
	/*
	struct sample_dev *dev = hwinfo + MINOR(inode->i_rdev);
	request_irq(dev->irq, ComBlk_IRQHandler, 0, "sysctrl", dev);
	*/
	ret_irq = request_irq(ComBlk_IRQn, ComBlk_IRQHandler, 0, "sysctrl", 0);
	if(ret_irq < 0){
		d_printk(1,"request_irq failed with %d", ret_irq);
		return -1;
	}

	/* One process at a time */
	if (sysctrl_lock ++ > 0) {
		ret = -EBUSY;
		d_printk(2, "lock=%d\n", sysctrl_lock);
		return ret;
	}
	else

	/* Increment the module use counter */
	try_module_get(THIS_MODULE);
	d_printk(2, "lock=%d\n", sysctrl_lock);
	return ret;
}

/*
** Device close
*/
static int sysctrl_release(struct inode *inode, struct file *file)
{
	/* Release interrupt and device */
 	free_irq(ComBlk_IRQn, 0);
	sysctrl_lock = 0;

	/* Decrement module use counter */
	module_put(THIS_MODULE);

	d_printk(2, "lock=%d\n", sysctrl_lock);
	return 0;
}

/* 
** Device read
*/
static ssize_t sysctrl_read(struct file *filp, char *buffer,
			 size_t length, loff_t * offset)
{
	/* Number of bytes written to buffer */
	int bytes_read = 0;
	/* Check for end of message */
	if(*Message_Ptr == 0) return 0;
	/* Put data into buffer */
	while(length && *Message_Ptr){
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	d_printk(3, "Read %d bytes, %d left", bytes_read, length);

	/* Return number of bytes read from buffer */
	return bytes_read;
}

/* 
** Device write
*/
static ssize_t sysctrl_write(struct file *filp, const char *buffer,
			  size_t length, loff_t * offset)
{
	int n=0;
	d_printk(3, "write(%p,%p,%d)", filp, buffer, length);
	/* Guess there are better ways of doing this...
	** But for now, this way the user program is the one
	** that tells us the status - if all data is written
	** then everything should be OK. Now, this is not 100%
	** fault free.
	 */
	if(g_programming_mode == AUTHENTICATE){
		g_isp_operation_busy = 1;
		g_buffer = &buffer;
		MSS_SYS_start_isp(MSS_SYS_PROG_AUTHENTICATE,page_read_handler,isp_completion_handler);
		while(g_isp_operation_busy){;} /* Waiting for isp_completion_handler */
		if(!g_isp_operation_busy){
			
			/* Error checking */
			if(g_error_flag == MSS_SYS_SUCCESS) {
				d_printk(1,"Authenticate succeeded, wrote %d bytes", n);
				return n;
			}
			else {
				d_printk(1, "Authenticate failed!");
				return -EINVAL;
			}
		}
		
	} 
	else if(g_programming_mode == VERIFY){
		return 0;
	}
	else if(g_programming_mode == PROGRAM){
		return 0;
	}
	else
	/* Return number of characters used */
	return n;
}

/* 
** IOCTL 
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int sysctrl_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long sysctrl_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	query_arg_t q;
	MSS_SYS_init(MSS_SYS_NO_EVENT_HANDLER);

	/* Check the command */
	switch(cmd)
	{
		case READ_IDCODE:
			d_printk(3, "Read serial number...\n");
			
			d_printk(3, "Passed init.\n");
			d_printk(3, "Before we get serial number, the variable is: %s", serial_number);
			status = MSS_SYS_get_serial_number(q.serial_number);
			d_printk(3, "Passed serial number get.\n");
			d_printk(3, "Status: %#x", status);
			if(MSS_SYS_SUCCESS == status){
				/* Replace with copy_to_user */
				d_printk(1, "Got serial number: %#02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
				, q.serial_number[15], q.serial_number[14]
				, q.serial_number[13], q.serial_number[12]
				, q.serial_number[11], q.serial_number[10]
				, q.serial_number[ 9], q.serial_number[ 8]
				, q.serial_number[ 7], q.serial_number[ 6]
				, q.serial_number[ 5], q.serial_number[ 4]
				, q.serial_number[ 3], q.serial_number[ 2]
				, q.serial_number[ 1], q.serial_number[ 0]);
				
				if( copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)) ) 
				  return -EACCES;
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
			break;
		case READ_USERCODE:
			status = MSS_SYS_get_user_code(q.user_code);
			if(MSS_SYS_SUCCESS == status){
				if( copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)) ) 
				  return -EACCES;
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
			break;
		case READ_DESIGN_VERSION:
			status = MSS_SYS_get_design_version(q.design_version);
			if(MSS_SYS_SUCCESS == status){
				if( copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)) ) 
				  return -EACCES;
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
			break;
		case PROG_AUTHENTICATE:
			d_printk(1, "Authentification...");
			g_programming_mode = AUTHENTICATE;
			break;
		case PROG_VERIFY:
			d_printk(1, "Verification...");
			g_programming_mode = VERIFY;
			break;
		case PROG_PROGRAM:
			d_printk(1, "Programming...");
			g_programming_mode = PROGRAM;
			break;
		default:
			return -EINVAL;
	
	}
	/*free_irq(ComBlk_IRQn, 0);*/
	return 0;
}

/*
** Device operations
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

	/* Check that the user has supplied a correct major number */
	if (sysctrl_major == 0) {
		printk(KERN_ALERT "%s: sysctrl_major can't be 0\n", __func__);
		ret = -EINVAL;
		goto Done;
	}

	/* Register device */
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
	/* Unregister device */
	unregister_chrdev(sysctrl_major, sysctrl_name);

	d_printk(1, "%s\n", "clean-up successful");
}

module_init(sysctrl_init_module);
module_exit(sysctrl_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars Bratrud, lars.bratrud@cern.ch");
MODULE_DESCRIPTION("System controller device driver");
