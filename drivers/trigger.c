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

#include "mrKernLogging.c"   // kernel debugging

/* Physical addresses */
static u32* rcu_trig_recv_physaddr = ((u32 *) 0x50040000);
static int  rcu_trig_recv_size     = 0x10; // size of 16 for testing 
/* Virtual addresses */
static u32* rcu_trig_recv_virtbase = NULL;

// Fixed major number
static uint rcuc_majorID = 254;

// Access control
static int driver_lock = 0;

static void __iomem *rcu;
static dev_t first; // Global variable for first minor device number
static struct cdev c_device; // Global variable for the char device structure
static struct class *c1; // Global variable for the device class


/*Internal functions*/
/* read from memory
 * the readl and readb functions are provided by the system and must be used to read from 
 * io memory initialized by ioremap (see init_module)
 * parameters:
 *  begin: start address
 *  size:  size of the buffer in BYTE
 *  buff:  pointer to buffer that receives data
 */
static int dcs_read (u32 begin, u32 size, u32* buff) 
{
  int i;
  u32 wordoffs=0;
  for (i=0;i<size/4;i++){ // do the multiples of 4 
    *(buff+wordoffs) = readl(begin+i*4);
    wordoffs++;    
  }
  i*=4;
  u32 byteoffs=0; 
  for (;i<size;i++) { // do the remaining bytes
    *(((char*)(buff+wordoffs))+byteoffs)=readb(begin+i);
    byteoffs++;
  }
  return 0;
}



/*
* Here comes file operation functions:
*/
// File open
static int rcuc_open(struct inode *inode, struct file *file)
{
	return 0;
}
// File close 
static int rcuc_close(struct inode *inode, struct file *file)
{
	return 0;
}
// File read
static ssize_t rcuc_read(struct file *file, 
                         char __user *buffer, 
                         size_t length, 
                         loff_t *offset)
{
	return 0;
}
// File write
static ssize_t rcuc_write(struct file *file, 
                          const char __user *buffer, 
                          size_t length, 
                          loff_t *offset)
{
	return 0;	
}
// Define our custom file operation structure
static struct file_operations rcuc_fops =
{
	.owner = THIS_MODULE,
	.open = rcuc_open,
	.release = rcuc_close,
	.read = rcuc_read,
	.write = rcuc_write
};

/*
* Module operation functions
*/
static int __init rcumodule_init(void)
{
	int iResult = 0; // holding result of operations
	u32* buffer;
	u32 u32Count = 1;
	
	
	// Register the driver
	iResult = register_chrdev(rcuc_majorID, "rcuc", &rcuc_fops);
	if (iResult < 0) {
		printk(KERN_INFO "module init: can't register driver\n");
		//mrlogmessage(LOG_ERROR, KERN_ERR "module init: can't register driver");
	}
	else{
		printk(KERN_INFO "module init: success!\n");
		//mrlogmessage(LOG_INFO, KERN_INFO, "module init: success!");
	}
	
	// Map physical address to virtual address
	if(rcu_trig_recv_size){
		rcu_trig_recv_virtbase = (u32*) ioremap_nocache( (u32 *)rcu_trig_recv_physaddr, rcu_trig_recv_size );
		printk("Remapped TRGRECV from 0x%p to 0x%p\n", rcu_trig_recv_physaddr, rcu_trig_recv_virtbase);
	}
	// try to read some stuff
	//dcs_read((u32)rcu_trig_recv_virtbase, u32Count, (u32*)buffer);

	*buffer = readl(rcu_trig_recv_virtbase);
	printk("read %lx, at 0x%p\n", *buffer, rcu_trig_recv_virtbase);
//	iResult = initRealBuffers();
	
	return iResult;
}

static void __exit rcumodule_exit(void)
{
	cdev_del(&c_device);
	device_destroy(c1, first);
	class_destroy(c1);
    unregister_chrdev_region(first, 1);
    iounmap(rcu);
	printk("Bye bye kernel\n"); // printed when succesfully closed
}

module_init(rcumodule_init);
module_exit(rcumodule_exit);

MODULE_AUTHOR("Lars Bratrud");
MODULE_DESCRIPTION("RCU2 registers driver");
MODULE_LICENSE("GPL");
