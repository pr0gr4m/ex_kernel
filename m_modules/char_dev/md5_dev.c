#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/scatterlist.h>
#include <crypto/hash.h>

#define MINOR_BASE			5
#define MINOR_NUM			1

#define DEV_NAME			"md5"
#define MAX_LEN				1024
#define MD5_LEN				16

static unsigned char plaintext[1024];

static char *hash_md5(unsigned char *buf);
static void hexdump(unsigned char *buf, unsigned int len);
static char *hexdump_as_str(unsigned char *buf, unsigned int len);

static int virtual_device_open(struct inode *, struct file *);
static int virtual_device_release(struct inode *, struct file *);
static ssize_t virtual_device_write(struct file *, const char *, size_t, loff_t *);
static ssize_t virtual_device_read(struct file *, char *, size_t, loff_t *);

static struct class *virtual_device_class = NULL;
static struct cdev *virtual_device = NULL;

static atomic_t virtual_device_usage = ATOMIC_INIT(1);
static int virtual_device_major = 0;
static char *buffer = NULL;

static struct file_operations vd_fops = {
	.read = virtual_device_read,
	.write = virtual_device_write,
	.open = virtual_device_open,
	.release = virtual_device_release
};

static int __init md5_init(void)
{
	int ret = 0;
	dev_t dev;

	virtual_device = cdev_alloc();
	ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DEV_NAME);
	if (ret != 0) {
		printk(KERN_ERR "alloc_chrdev_region = %d\n", ret);
		return -1;
	}

	virtual_device_major = MAJOR(dev);
	dev = MKDEV(virtual_device_major, MINOR_BASE);
	printk("[Message] major num: %d\n", virtual_device_major);

	cdev_init(virtual_device, &vd_fops);
	virtual_device->owner = THIS_MODULE;

	ret = cdev_add(virtual_device, dev, MINOR_NUM);
	if (ret != 0) {
		printk(KERN_ERR "cdev_add = %d\n", ret);
		goto OUT2;
	}

	virtual_device_class = class_create(THIS_MODULE, "md5_virtual_device");
	if (IS_ERR(virtual_device_class)) {
		printk(KERN_ERR "class_create\n");
		goto OUT;
	}

	device_create(virtual_device_class, NULL, MKDEV(virtual_device_major, MINOR_BASE), NULL, DEV_NAME);

	buffer = (char *)kzalloc(64, GFP_KERNEL);
	if (buffer == NULL) {
		printk(KERN_ERR "[Message] kzalloc failed\n");
		goto OUT;
	}

	return 0;

OUT:
	cdev_del(virtual_device);
OUT2:
	unregister_chrdev_region(dev, MINOR_NUM);
	return -1;
}

static void __exit md5_exit(void)
{
	dev_t dev = MKDEV(virtual_device_major, MINOR_BASE);

	device_destroy(virtual_device_class, dev);
	class_destroy(virtual_device_class);
	cdev_del(virtual_device);
	unregister_chrdev_region(dev, MINOR_NUM);
}


static int virtual_device_open(struct inode *inode, struct file *filp)
{
	printk("[Message] before open: %d\n", virtual_device_usage.counter);

	if (!atomic_dec_and_test(&virtual_device_usage)) {
		// already used..
		atomic_inc(&virtual_device_usage);
		return -EBUSY;
	}
	printk("[Message] after open: %d\n", virtual_device_usage.counter);
	return 0;
}

static int virtual_device_release(struct inode *inode, struct file *filp)
{
	printk("[Message] before release: %d\n", virtual_device_usage.counter);
	atomic_inc(&virtual_device_usage);
	printk("[Message] after release: %d\n", virtual_device_usage.counter);
	return 0;
}

static ssize_t virtual_device_write(struct file *filp, const char __user *buf, 
		size_t count, loff_t *f_pos)
{
	printk("[Message] write function called\n");
	copy_from_user(plaintext, buf, count);		// copy to plain data
	return count;
}

static ssize_t virtual_device_read(struct file *filp, char __user *buf, 
		size_t count, loff_t *f_pos)
{
	char *p;
	printk("[Message] read function called\n");
	p = hash_md5(plaintext);
	copy_to_user(buf, p, MD5_LEN);
	kfree(p);
	return count;
}

static void hexdump(unsigned char *buf, unsigned int len)
{
	printk("[Message] ");
	while (len--)
		printk("%02x", *buf++);
	printk("\n");
}

static char *hexdump_as_str(unsigned char *buf, unsigned int len)
{
	char *str = (char *)kmalloc(len, GFP_KERNEL);
	while (len--)
		strcat(str++, buf++);
	return str;
}

static char *hash_md5(unsigned char *plaintext)
{
	int i, success = 0;
	char *result;
	struct crypto_shash *handle = NULL;
	struct shash_desc *shash = NULL;

	printk("hash_md5 function called\n");

	result = (char *)kzalloc(MD5_LEN, GFP_KERNEL);

	handle = crypto_alloc_shash("md5", 0, 0);
	if (IS_ERR(handle))
		goto EXIT_ERROR;
	shash = kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(handle),
				GFP_KERNEL);
	if (shash == NULL)
		goto EXIT_ERROR;

	shash->tfm = handle;

	success = crypto_shash_init(shash);
	if (success < 0)
		goto EXIT_ERROR;

	success = crypto_shash_update(shash, plaintext, strlen(plaintext));
	if (success < 0)
		goto EXIT_ERROR;

	success = crypto_shash_final(shash, result);
	if (success < 0)
		goto EXIT_ERROR;

	printk("[Message] ---------- test md5 ----------\n");
	i = crypto_shash_digestsize(shash->tfm);
	hexdump(result, i);
	printk("[Message] Length: %d\n", i);

EXIT_ERROR:
	if (shash)
		kfree(shash);
	if (handle)
		crypto_free_shash(handle);

	return result;
}

module_init(md5_init);
module_exit(md5_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pr0gr4m");
MODULE_DESCRIPTION("MD5 hash device");
