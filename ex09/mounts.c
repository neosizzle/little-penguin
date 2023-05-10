#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dcache.h>
#include <linux/fs.h>

int module_start(void)
{
	
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;

	list_for_each_entry(mnt, &ns->list, mnt_list) {
		printk("sheesh is mounted\n");
	}
    return 0;
}

void module_end(void)
{
	
}

module_init(module_start);
module_exit(module_end);