#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int simple_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Hello proc\n");
	return 0;
}

static int simple_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, simple_proc_show, NULL);
}

static const struct proc_ops simple_proc_fops = {
	.proc_open = simple_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};

static int __init simple_init(void)
{
	proc_create("simple", 0, NULL, &simple_proc_fops);
	return 0;
}

static void __exit simple_exit(void)
{
	remove_proc_entry("simple", NULL);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
