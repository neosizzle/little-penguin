#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dcache.h>

int init_module(void)
{
	
    struct dentry *curdentry;

    list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
    {
        if ( curdentry->d_flags & DCACHE_MOUNTED)
            printk("%s is mounted", curdentry->d_name.name);
    }
    return 0;
}

void exit_module(void)
{
	
}

module_init(init_module);
module_exit(exit_module);