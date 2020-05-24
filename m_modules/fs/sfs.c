#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>


#define SFS_MAGIC	0x20200524

#define NCOUNTERS	4


static atomic_t counters[NCOUNTERS];

static int sfs_open(struct inode *inode, struct file *filp)
{
	if (inode->i_ino > NCOUNTERS)
		return -ENODEV;

	filp->private_data = counters + inode->i_ino - 1;
	return 0;
}


#define TMPSIZ	32

static ssize_t sfs_read_file(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	int v, len;
	char tmp[TMPSIZ];
	atomic_t *counter = (atomic_t *)filp->private_data;

	v = atomic_read(counter);

	if (*f_pos > 0)
		--v;
	else
		atomic_inc(counter);

	len = snprintf(tmp, TMPSIZ, "%d\n", v);
	if (*f_pos > len)
		return 0;

	if (count > len - *f_pos)
		count = len - *f_pos;

	if (copy_to_user(buf, tmp + *f_pos, count))
		return -EFAULT;

	*f_pos += count;
	return count;
}

static ssize_t sfs_write_file(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	char tmp[TMPSIZ] = { 0, };
	atomic_t *counter = (atomic_t *)filp->private_data;

	if (*f_pos != 0) {
		printk(KERN_ERR "%s: f_pos", __func__);
		return -EINVAL;
	}

	if (count >= TMPSIZ) {
		printk(KERN_ERR "%s: count", __func__);
		return -EINVAL;
	}

	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	atomic_set(counter, simple_strtol(tmp, NULL, 10));
	return count;
}


static struct file_operations sfs_file_ops = {
	.open = sfs_open,
	.read = sfs_read_file,
	.write = sfs_write_file,
};


// Simple Filesystem Tree
struct tree_descr root_files[] = {
	{ NULL, NULL, 0 },

	{
		.name = "counter0",
		.ops = &sfs_file_ops,
		.mode = S_IWUSR | S_IRUGO
	},

	{
		.name = "counter1",
		.ops = &sfs_file_ops,
		.mode = S_IWUSR | S_IRUGO
	},

	{
		.name = "counter2",
		.ops = &sfs_file_ops,
		.mode = S_IWUSR | S_IRUGO
	},

	{
		.name = "counter3",
		.ops = &sfs_file_ops,
		.mode = S_IWUSR | S_IRUGO
	},

	{ "", NULL, 0 }
};

static int sfs_fill_super(struct super_block *sb, struct fs_context *fc)
{
	return simple_fill_super(sb, SFS_MAGIC, root_files);
}

static int sfs_get_tree(struct fs_context *fc)
{
	return get_tree_single(fc, sfs_fill_super);
}

static const struct fs_context_operations sfs_context_ops = {
	.get_tree = sfs_get_tree,
};

static int sfs_init_fs_context(struct fs_context *fc)
{
	fc->ops = &sfs_context_ops;
	return 0;
}

static struct file_system_type sfs_type = {
	.owner = THIS_MODULE,
	.name = "sfs",
	.init_fs_context = sfs_init_fs_context,
	.kill_sb = kill_litter_super,
};


static int __init sfs_init(void)
{
	int i;
	for (i = 0; i < NCOUNTERS; i++)
		atomic_set(counters + i, 0);
	return register_filesystem(&sfs_type);
}

static void __exit sfs_exit(void)
{
	unregister_filesystem(&sfs_type);
}

module_init(sfs_init);
module_exit(sfs_exit);
MODULE_LICENSE("GPL");
