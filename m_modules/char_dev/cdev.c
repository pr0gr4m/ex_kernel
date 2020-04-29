#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t virtual_device_number;
static unsigned int chardev_major;
static struct class *chardev_class = NULL;
static struct cdev *virtual_device = NULL;

int virtual_device_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int virtual_device_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t virtual_device_write(struct file *filp, const char *buf,
		size_t count, loff_t *f_pos)
{
	printk("virutal_device write called\n");
	return count;
}

ssize_t virtual_device_read(struct file *filp, char *buf,
		size_t count, loff_t *f_pos)
{
	printk("virtual_device read called\n");
	return count;
}

static struct file_operations vd_fops = {
	.read = virtual_device_read,
	.write = virtual_device_write,
	.open = virtual_device_open,
	.release = virtual_device_release
};

int __init dev_init(void)
{
	virtual_device = cdev_alloc();
	alloc_chrdev_region(&virtual_device_number, 10, 1, "virtual_device");

	chardev_major = MAJOR(virtual_device_number);
	virtual_device_number = MKDEV(chardev_major, 10);

	cdev_init(virtual_device, &vd_fops);
	virtual_device->owner = THIS_MODULE;
	cdev_add(virtual_device, virtual_device_number, 1);

	chardev_class = class_create(THIS_MODULE, "virtual_device");
	device_create(chardev_class, NULL, MKDEV(chardev_major, 10), NULL, "virtual_device");

	printk("[Module Message] Major: %d\n", MAJOR(virtual_device_number));
	printk("[Module Message] Minor: %d\n", MINOR(virtual_device_number));
	
	return 0;
}

void __exit dev_exit(void)
{
	device_destroy(chardev_class, MKDEV(chardev_major, 10));
	class_destroy(chardev_class);
	cdev_del(virtual_device);
	unregister_chrdev_region(virtual_device_number, 1);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("pr0gr4m");
MODULE_DESCRIPTION("cdev example");
