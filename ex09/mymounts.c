#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/string.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jng");

int my_open(struct inode *, struct file *)
{
	return 0;
}

ssize_t my_read(struct file *, char __user *buffer, size_t, loff_t * offset)
{
	struct dentry *curdentry;
	char *res = kmalloc(69420, GFP_USER);

	if (*offset >= 69420) return 0;
	strcat(res, "root   ");
	strcat(res,  current->fs->root.mnt->mnt_root->d_name.name);
	strcat(res, "\n");
	list_for_each_entry(curdentry,
			    &current->fs->root.mnt->mnt_root->d_subdirs,
			    d_child)
	{
		if (curdentry->d_flags & DCACHE_MOUNTED)
		strcat(res, curdentry->d_name.name);
		strcat(res, "	/");
		strcat(res,  curdentry->d_name.name);
		strcat(res, "\n");
	}
	strcat(res, "");
	*offset = 69420;
	copy_to_user(buffer, res, strlen(res));
	return 69420;
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