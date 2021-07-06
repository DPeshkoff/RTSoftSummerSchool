#include <linux/kernel.h> // base
#include <linux/module.h> // modules
#include <linux/init.h> // init and cleanup
#include <linux/fs.h> // register and delete driver
#include <linux/cdev.h> // for file operations
#include <stdbool.h> // c bool

#define DEVICE_NAME "ADC"
#define SUCCESS 0

MODULE_AUTHOR("DPeshkoff");
MODULE_DESCRIPTION("ADC");
MODULE_VERSION("0.0.1");
MODULE_LICENSE("GPL");

// Global variables
static int32_t major;
static int32_t counter = 0;
static bool is_open = false;

// Prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

// Operation callbacks
static struct file_operations fops = { .open = device_open,
				       .close = device_close,
				       .read = device_read };

// Module initialization
int init_module(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		printk(KERN_ERR "ADC module cannot be initialized. Reason: %d\n", major);
		return major;
	}
	return SUCCESS;
}

// Open device
static int device_open(struct inode *inode, struct file *file)
{
	if (is_open) {
		return -EBUSY;
	}

	is_open = true;

	return SUCCESS;
}

// Close device
static int device_close(struct inode *inode, struct file *file)
{
	is_open = false;

	return SUCCESS;
}

// Read
static ssize_t device_read(struct file *filp, char *buffer, size_t length,
			   loff_t *offset)
{
	size_t read = sprintf(buffer, "%d\n", counter++);

	return read;
}

// Cleanup module
void cleanup_module(void)
{
	if (!is_open) {
		unregister_chrdev(major, DEVICE_NAME);
		printk(KERN_INFO "ADC module cleaned successfully!\n");
	} else {
		printk(KERN_ALERT "ADC module cannot be stoped - is opened!\n");
	}
}
