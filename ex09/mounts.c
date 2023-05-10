#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dcache.h>

int module_start(void)
{
	
    struct dentry *curdentry;

    list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child)
    {
        if ( curdentry->d_flags & DCACHE_MOUNTED)
            printk("%s is mounted", curdentry->d_name.name);
    }
    return 0;
}

void module_end(void)
{
	
}

module_init(module_start);
module_exit(module_end);