#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

int my_open(struct inode *, struct file *)
{
	return 0;
}

ssize_t my_read(struct file *, char __user *, size_t, loff_t *)
{
	printk(KERN_INFO "proc device read\n");
	return 0;
}

struct proc_ops myops = {
	.proc_open = &my_open,
	.proc_read = &my_read,
	.owner = THIS_MODULE,
}

static struct proc_dir_entry *mymnt_dev;

int module_start(void)
{
	// create proc device
	mymnt_dev = proc_create("mymounts",0660,NULL,&myops);
	return 0;
}

void module_end(void)
{
	proc_remove(mymnt_dev);
}

module_init(module_start);
module_exit(module_end)