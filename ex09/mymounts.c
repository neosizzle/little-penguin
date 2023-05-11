#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jng");

int my_open(struct inode *, struct file *)
{
	return 0;
}

ssize_t my_read(struct file *, char __user *, size_t, loff_t *)
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

struct proc_ops myops = {
	.proc_open = &my_open,
	.proc_read = &my_read,
};

static struct proc_dir_entry *mymnt_dev;

int module_start(void)
{
	printk("mymounts module loaded \n");
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