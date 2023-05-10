#include <linux/nsproxy.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/dcache.h>
#include <linux/init.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/mnt_namespace.h>
#include <linux/fs_struct.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/wait.h>
#include <linux/fs_pin.h>

char *file_name = "mymounts";

ssize_t read_proc(struct file *filp, char *buf, size_t len, loff_t *offp )
{
	struct dentry *curdentry;
	printk("root   %s", current->fs->root.mnt->mnt_root->d_name.name);
	list_for_each_entry(curdentry,
			    &current->fs->root.mnt->mnt_root->d_subdirs,
			    d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
			printk("%s    /%s", curdentry->d_name.name,
					      curdentry->d_name.name);
	}
	return 0;
}

static int open_proc(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations proc_fops = {
	.read = read_proc,
	.open = open_proc,
};

int proc_init(void)
{
	proc_create(file_name, 0, NULL, &proc_fops);

	return 0;
}

void proc_cleanup(void)
{
	remove_proc_entry(file_name, NULL);
}

module_init(proc_init);
module_exit(proc_cleanup);

MODULE_DESCRIPTION("Proc driver");
MODULE_AUTHOR("Knage");
MODULE_LICENSE("GPL");