#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/debugfs.h> /* debugfs*/
#include <linux/miscdevice.h>
#include <linux/jiffies.h>/* jiffies*/
#include <linux/mutex.h> /* Mutex*/

MODULE_AUTHOR("jng");
MODULE_LICENSE("GPL");

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
static char foo_data[PAGE_SIZE + 1] = "";
static int foo_data_size = 0;
struct mutex foo_mutex;
static struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

static ssize_t foo_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int length_to_read;
	int copy_fail;

	if (*offset >= foo_data_size)
		return 0;
	length_to_read = len > foo_data_size ? foo_data_size : len;
	mutex_lock(&foo_mutex);
	copy_fail = copy_to_user(buffer, foo_data + *offset, length_to_read);
	mutex_unlock(&foo_mutex);
	if (copy_fail < 0)
		return copy_fail;
	length_to_read -= copy_fail;
	*offset += length_to_read;
	return length_to_read;
}

static ssize_t foo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int len_to_cpy;
	int failed_to_cpy;

	if (*offset >= PAGE_SIZE)
	{
		printk(KERN_INFO "foo: *offset >= PAGE_SIZE at offset %lld len %lu \n", *offset, len);
		return -ENOSPC;
	}
	if (len + *offset > PAGE_SIZE)
	{
		printk(KERN_INFO "foo: len + *offset > PAGE_SIZE at offset %lld len %lu \n", *offset, len);
		return -ENOSPC;
	}

	len_to_cpy = len;
	mutex_lock(&foo_mutex);
	failed_to_cpy = copy_from_user(foo_data + *offset, buffer, len_to_cpy);
	mutex_unlock(&foo_mutex);
	if (failed_to_cpy < 0)
		return failed_to_cpy;
	if (failed_to_cpy != 0)
		printk(KERN_INFO "foo: failed to copy last %d bytes\n", failed_to_cpy);
	// len_to_cpy -= failed_to_cpy;
	foo_data_size += len_to_cpy;
	*offset += len_to_cpy;
	return len_to_cpy;
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
	int msg_len;
	int size_to_read;
	int res;

	sprintf(jiffy_str, "%lu", jiffies);
	msg_len = strlen(jiffy_str);
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
	struct dentry *id_file;
	struct dentry *jiffy_file;
	struct dentry *foo_file;

	// create debugfs directory
	fortytwo = debugfs_create_dir("fortytwo", NULL);
	if (fortytwo == NULL)
	{
		printk(KERN_INFO "debugfs_create_dir error\n");
		return -1;
	}

	// create id file
	id_file = debugfs_create_file("id", 0666,
                                   fortytwo, NULL,
                                   &id_fops);
	if (id_file == NULL)
	{
		printk(KERN_INFO "id debugfs_create_dir error\n");
		return -1;
	}

	// create jiffies file
	jiffy_file = debugfs_create_file("jiffies", 0444,
                                   fortytwo, NULL,
                                   &jiffies_fops);
	if (jiffy_file == NULL)
	{
		printk(KERN_INFO "jiffy debugfs_create_dir error\n");
		return -1;
	}

	// crate foo file
	mutex_init(&foo_mutex);
	foo_file = debugfs_create_file("foo", 0644,
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
