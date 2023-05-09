#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
// Dont have a license, LOL
MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user,
						 size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user,
						  size_t size, loff_t *offs);
static struct file_operations myfd_fops = {
	.owner = THIS_MODULE, .read = &myfd_read, .write = &myfd_write};
static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR, .name = "reverse", .fops = &myfd_fops};
char str[PAGE_SIZE];
char *tmp;
static int __init myfd_init(void)
{
	int retval;
	retval = misc_register(&(*(&(myfd_device))));
	return 0;
}
static void __exit myfd_cleanup(void)
{
	// TODO ; cleanup function
	misc_deregister(&myfd_device);
}

// ssize_t simple_read_from_buffer(char __user *user, size_t size, loff_t *offset, char *tmp, int len)
// {
// 	int msg_len = strlen(tmp);
// 	int size_to_read;
// 	int res;

// 	if (*offset >= msg_len)
// 		return 0;

// 	size_to_read = len > msg_len ? msg_len : msg_len - *offset;
// 	res = copy_to_user(buffer, login + *offset, size_to_read);
// 	if (res < 0)
// 		return res;

// 	size_to_read -= res;
// 	*offset += size_to_read;
// 	return size_to_read;
// }

// ssize_t simple_write_to_buffer(char __user *user, size_t size, loff_t *offset, char *tmp, int len)
// {
// 	int len_to_cpy;
// 	int failed_to_cpy;

// 	if (*offset >= PAGE_SIZE)
// 	{
// 		printk(KERN_INFO "reverse: *offset >= PAGE_SIZE at offset %d len %d \n", *offset, len);
// 		return -ENOSPC;
// 	}
// 	if (len + *offset > PAGE_SIZE)
// 	{
// 		printk(KERN_INFO "reverse: len + *offset > PAGE_SIZE at offset %d len %d \n", *offset, len);
// 		return -ENOSPC;
// 	}

// 	len_to_cpy = len;
// 	failed_to_cpy = copy_from_user(tmp + *offset, user, len_to_cpy);
// 	if (failed_to_cpy < 0)
// 		return failed_to_cpy;
// 	if (failed_to_cpy != 0)
// 		printk(KERN_INFO "foo: failed to copy last %d bytes\n", failed_to_cpy);
// 	// len_to_cpy -= failed_to_cpy;
// 	*offset += len_to_cpy;
// 	return len_to_cpy;
// }

ssize_t myfd_read(struct file *fp,
				  char __user *user,
				  size_t size,
				  loff_t *offs)
{
	size_t t, i;
	char *tmp2;
	/***************
	 * Malloc like a boss
	 ***************/
	tmp2 = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	tmp = tmp2;
	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++)
	{
		tmp[i] = str[t];
	}
	tmp[i] = 0x0;
	return simple_read_from_buffer(user, size, offs, tmp, i);
}
ssize_t myfd_write(struct file *fp,
				   const char __user *user,
				   size_t size,
				   loff_t *offs)
{
	ssize_t res;
	res = 0;
	res = simple_write_to_buffer(str, size, offs, user, size) + 1;
	// 0x0 = ’\0’
	str[size + 1] = 0x0;
	return res;
}
module_init(myfd_init);
module_exit(myfd_cleanup);