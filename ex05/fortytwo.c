#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h> // class_create, class_destroy

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

// TODO syscalls

static void fortytwo_cleanup(void)
{
	//	free major number
	unregister_chrdev_region(major, 1);

	//	unregister char device
	unregister_chrdev(major, DEVICE_NAME);

	//	delete device node

	//	delete device class node
	class_destroy(fortytwo_class);
}

static int fortytwo_init(void)
{
	//	get major and minor number /proc/devices
	int major_nums_status = alloc_chrdev_region(&major , ++minor, 1, DEVICE_NAME);
	if (major_nums_status < 0)
	{
		printk(KERN_INFO "Major and minor allocation failed.\n");
		return -1;
	}

	printk("majornum success %d, num %d\n", major_nums_status, major);

	//	create device class node /sys/classes
	fortytwo_class = class_create(THIS_MODULE, "fortytwo_class");
	printk("class create success %s \n", fortytwo_class->name);

	//  register char device in kernel @ kernelspace
	register_chrdev(major, DEVICE_NAME, struct file_operations *fops);

	//	create device node /dev/device
	return 0;
}

static void fortytwo_exit(void)
{
	fortytwo_cleanup();
	printk("42module exit\n");
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);