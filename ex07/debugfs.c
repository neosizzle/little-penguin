#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/debugfs.h> /* debugfs*/
#include <linux/miscdevice.h>
// #include <linux/stdio.h> /* sprintf*/
#include <linux/jiffies.h>/* jiffies*/

void cleanup(struct dentry * debugfs)
{
	// void debugfs_remove_recursive(struct dentry *dentry);
	debugfs_remove_recursive(debugfs);
}

/**
 * foo file functions
*/
static ssize_t foo_read(struct file*, char*, size_t, loff_t*);
static ssize_t foo_write(struct file*, const char*, size_t, loff_t*);
static char foo_data[PAGE_SIZE] = "ben ten";
static int foo_data_size = 7;
static struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static ssize_t foo_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int length_to_read;
	int copy_fail;

	printk(KERN_INFO "Read /sys/kernel/debug/fortytwo/foo of length %lu with offset %lld\n", len, *offset);
	length_to_read = len > foo_data_size ? foo_data_size : foo_data_size - *offset;
	copy_fail = copy_to_user(buffer, foo_data + *offset, 7);
	if (copy_fail < 0)
		return copy_fail;
	length_to_read -= copy_fail;
	*offset += length_to_read;
	return length_to_read;
}

static ssize_t foo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "foo: write executed\n");
	return len;
}

/**
 * jiffy file functions
*/
static ssize_t jiffy_read(struct file*, char*, size_t, loff_t*);
static struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffy_read,
};

static ssize_t jiffy_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	char jiffy_str[1024 + 1];
	sprintf(jiffy_str, "%lu", jiffies);
	int msg_len = strlen(jiffy_str);
	int size_to_read;
	int res;

	printk(KERN_INFO "Read /sys/kernel/debug/fortytwo/id of length %lu with offset %lld\n", len, *offset);
	if (*offset >= msg_len)
		return 0;

	size_to_read = len > msg_len ? msg_len : msg_len - *offset;
	res = copy_to_user(buffer, jiffy_str + *offset, size_to_read);
	if (res < 0)
		return res;

	size_to_read -= res;
	*offset += size_to_read;
	return size_to_read;
}

/**
 * id file functions
*/
static ssize_t id_read(struct file*, char*, size_t, loff_t*);
static ssize_t id_write(struct file*, const char*, size_t, loff_t*);
static char* login = "jng";
static struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (!n)
		return (0);
	while ((*s1) && (*s1 == *s2) && --n)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

static ssize_t id_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {
	char message[1024 + 1];
	int res;

	printk(KERN_INFO "write /sys/kernel/debug/fortytwo/id of length %lu\n", len);
	res = copy_from_user(message, buffer, len);
	if ( res < 0)
	{
		printk(KERN_INFO "Copy from user error\n");
		return -EFAULT;
	}
	if (ft_strncmp(login, message, len > strlen(login) ? len : strlen(login)) != 0)
	{
		printk(KERN_INFO "invalid argument\n");
		return -EINVAL;
	}
	printk(KERN_INFO "correct value\n");
	return len;
}

static ssize_t id_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	int msg_len = strlen(login);
	int size_to_read;
	int res;

	printk(KERN_INFO "Read /sys/kernel/debug/fortytwo/id of length %lu with offset %lld\n", len, *offset);
	if (*offset >= msg_len)
		return 0;

	size_to_read = len > msg_len ? msg_len : msg_len - *offset;
	res = copy_to_user(buffer, login + *offset, size_to_read);
	if (res < 0)
		return res;

	size_to_read -= res;
	*offset += size_to_read;
	return size_to_read;
}


/**
 * Module
*/
struct dentry *fortytwo;

int init_module(void)
{
	printk(KERN_INFO "Hello world !\n");

	// create debugfs directory
	fortytwo = debugfs_create_dir("fortytwo", NULL);
	if (fortytwo == NULL)
	{
		printk(KERN_INFO "debugfs_create_dir error\n");
		return -1;
	}

	// create id file
	struct dentry *id_file = debugfs_create_file("id", 0666,
                                   fortytwo, NULL,
                                   &id_fops);
	if (id_file == NULL)
	{
		printk(KERN_INFO "id debugfs_create_dir error\n");
		return -1;
	}

	// create jiffies file
	struct dentry *jiffy_file = debugfs_create_file("jiffies", 0444,
                                   fortytwo, NULL,
                                   &jiffies_fops);
	if (jiffy_file == NULL)
	{
		printk(KERN_INFO "jiffy debugfs_create_dir error\n");
		return -1;
	}

	// crate foo file
	struct dentry *foo_file = debugfs_create_file("foo", 0444,
                                   fortytwo, NULL,
                                   &foo_fops);
	if (foo_file == NULL)
	{
		printk(KERN_INFO "foo debugfs_create_dir error\n");
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
	cleanup(fortytwo);
	printk(KERN_INFO "Cleaning up module.\n");
}

// By setting the license to GPL, so people know that the code is open-source
MODULE_LICENSE("GPL");