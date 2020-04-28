#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/current.h>
#include "ioctl.h"

MODULE_LICENSE("GPL");

#define DEVICE_NAME		"chardev"

static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 1;
static unsigned int chardev_major;
static struct cdev chardev_cdev;
static struct class *chardev_class = NULL;

static int chardev_open(struct inode *, struct file *);
static int chardev_release(struct inode *, struct file *);
static ssize_t chardev_read(struct file *, char *, size_t, loff_t *);
static ssize_t chardev_write(struct file *, const char *, size_t, loff_t *);
static long chardev_ioctl(struct file *, unsigned int, unsigned long);

struct file_operations chardev_fops = {
	.open = chardev_open,
	.release = chardev_release,
	.read = chardev_read,
	.write = chardev_write,
	.unlocked_ioctl = chardev_ioctl,
};

static int __init chardev_init(void)
{
	int alloc_ret = 0, cdev_err = 0, minor = 0;
	dev_t dev;

	alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DEVICE_NAME);
	if (alloc_ret != 0) {
		printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
		return -1;
	}

	// get the major number value in dev
	chardev_major = MAJOR(dev);
	dev = MKDEV(chardev_major, MINOR_BASE);

	// initialize a cdev structure
	cdev_init(&chardev_cdev, &chardev_fops);
	chardev_cdev.owner = THIS_MODULE;

	// add a char device to the system
	cdev_err = cdev_add(&chardev_cdev, dev, MINOR_NUM);
	if (cdev_err != 0) {
		printk(KERN_ERR "cdev_add = %d\n", cdev_err);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	chardev_class = class_create(THIS_MODULE, "chardev");
	if (IS_ERR(chardev_class)) {
		printk(KERN_ERR "class_create\n");
		cdev_del(&chardev_cdev);
		unregister_chrdev_region(dev, MINOR_NUM);
		return -1;
	}

	device_create(chardev_class, NULL, MKDEV(chardev_major, minor),
			NULL, "chardev%d", minor);
	
	return 0;
}

static void __exit chardev_exit(void)
{
	int minor = 0;
	dev_t dev = MKDEV(chardev_major, MINOR_BASE);

	device_destroy(chardev_class, MKDEV(chardev_major, minor));
	class_destroy(chardev_class);
	cdev_del(&chardev_cdev);
	unregister_chrdev_region(dev, MINOR_NUM);
}

static int chardev_open(struct inode *inode, struct file *file)
{
	printk("chardev_open() has been called\n");
	return 0;
}

static int chardev_release(struct inode *inode, struct file *file)
{
	printk("chardev_release() has been called\n");
	return 0;
}

static ssize_t chardev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("chardev_read() has been called\n");
	return count;
}

static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("chardev_write() has been called\n");
	return count;
}

static struct ioctl_info info;

static long chardev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("chardev_ioctl() has been called\n");

	switch (cmd) {
		case IOCTL_READ:
			if (copy_to_user((void __user *)arg, &info, sizeof(info))) {
				return -EFAULT;
			}
			break;

		case IOCTL_WRITE:
			if (copy_from_user(&info, (void __user *)arg, sizeof(info))) {
				return -EFAULT;
			}
			break;

		case IOCTL_STATUS:
			printk("info : [%ld/%s]\n", info.size, info.buf);
			break;

		case IOCTL_RW:
			if (copy_from_user(&info, (void __user *)arg, sizeof(info))) {
				return -EFAULT;
			}

			sprintf(info.buf, "%s %s", info.buf, "hello");
			info.size += strlen(" hello");

			if (copy_to_user((void __user *)arg, &info, sizeof(info))) {
				return -EFAULT;
			}
			break;

		default:
			printk(KERN_WARNING "unsupported command %d\n", cmd);
			return -EFAULT;
	}

	return 0;
}

module_init(chardev_init);
module_exit(chardev_exit);
