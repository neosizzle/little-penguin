#include <linux/cdev.h> /* cdev_add, cdev_init */
#include <linux/device.h>
#include <linux/fs.h> /* register_chrdev, unregister_chrdev */
#include <linux/module.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */

#define DEVICE_NAME "fortytwo"

MODULE_AUTHOR("jng");
MODULE_LICENSE("GPL");

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int major = -1;
static int minor = -1;

struct class* fortytwo_class = NULL;
static struct cdev mycdev;
static char* login = "jng\n";

// TODO syscalls
static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Rickroll device opened\n");
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {

   printk(KERN_INFO "Sorry, rickroll is read only\n");
   return -EFAULT;
}

static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Fortytwo device closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	int msg_len = strlen(login);
	int size_to_read;
	int res;

	if (*offset > msg_len)
		return 0;

	size_to_read = len > msg_len ? msg_len : msg_len - *offset;
	res = copy_to_user(buffer, login + *offset, size_to_read);
	if (res < 0)
		return res;

	size_to_read -= retval;
	*offset += size_to_read;
	return size_to_read;
}

static void fortytwo_cleanup(void)
{
	//	unregister char device
	cdev_del(&mycdev);

	//	delete device node
	device_destroy(fortytwo_class, 1);

	//	delete device class node
	class_destroy(fortytwo_class);

	//	free major number
	unregister_chrdev_region(major, 1);
}

static int fortytwo_init(void)
{
	//	get major and minor number /proc/devices
	int major_nums_status = alloc_chrdev_region(&major , ++minor, 1, DEVICE_NAME);
	if (major_nums_status < 0)
	{
		fortytwo_cleanup();
		printk(KERN_INFO "Major and minor allocation failed.\n");
		return -1;
	}

	//	create device class node /sys/classes
	fortytwo_class = class_create(THIS_MODULE, "fortytwo_class");
	if (fortytwo_class == NULL)
	{
		fortytwo_cleanup();
		printk(KERN_INFO "Class creation failed.\n");
		return -1;
	}

	//	create device node /dev/device
	if (device_create(fortytwo_class, NULL, major, NULL, DEVICE_NAME) == NULL)
	{
		fortytwo_cleanup();
		printk(KERN_INFO "Device creation failed.\n");
		return -1;
	}

	//  register char device in kernel @ kernelspace
	cdev_init(&mycdev, &fops);
	if (cdev_add(&mycdev, major, 1) == -1)
	{
		fortytwo_cleanup();
		printk(KERN_INFO "cdev registration failed.\n");
		return -1;
	}

	return 0;
}

static void fortytwo_exit(void)
{
	fortytwo_cleanup();
	printk("42module exit\n");
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);