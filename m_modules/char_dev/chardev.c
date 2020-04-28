#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <linux/uaccess.h>
 
MODULE_LICENSE("GPL");
 
#define DEVICE_NAME "chardev"
#define BUF_LEN		1024
     
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;
static unsigned int chardev_major;
static struct cdev chardev_cdev;
static struct class *chardev_class = NULL;
 
static int     chardev_open(struct inode *, struct file *);
static int     chardev_release(struct inode *, struct file *);
static ssize_t chardev_read(struct file *, char *, size_t, loff_t *);
static ssize_t chardev_write(struct file *, const char *, size_t, loff_t *);
 
struct file_operations chardev_fops = {
    .open    = chardev_open,
    .release = chardev_release,
    .read    = chardev_read,
    .write   = chardev_write,
};
 
struct data {
    unsigned char buffer[BUF_LEN];
};
 
int __init init_chardev(void)
{
    int alloc_ret = 0;
    int cdev_err = 0;
    int minor;
    dev_t dev;
     
    alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DEVICE_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR  "alloc_chrdev_region = %d\n", alloc_ret);
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
        printk(KERN_ERR  "cdev_add = %d\n", alloc_ret);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }
 
    chardev_class = class_create(THIS_MODULE, "chardev");
    if (IS_ERR(chardev_class)) {
        printk(KERN_ERR  "class_create\n");
        cdev_del(&chardev_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }
 
    for (minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_create(chardev_class, NULL, MKDEV(chardev_major, minor), NULL, "chardev%d", minor);
    }
 
    return 0;
}
 
void __exit exit_chardev(void)
{
    int minor; 
    dev_t dev = MKDEV(chardev_major, MINOR_BASE);
     
    for (minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_destroy(chardev_class, MKDEV(chardev_major, minor));
    }
 
    class_destroy(chardev_class);
    cdev_del(&chardev_cdev);
    unregister_chrdev_region(dev, MINOR_NUM);
}
 
static int chardev_open(struct inode *inode, struct file *file)
{
    struct data *p = kmalloc(sizeof(struct data), GFP_KERNEL);
     
    if (p == NULL) {
        printk(KERN_ERR  "kmalloc - Null");
        return -ENOMEM;
    }
 
    file->private_data = p;
    return 0;
}
 
static int chardev_release(struct inode *inode, struct file *file)
{
    if (file->private_data) {
        kfree(file->private_data);
        file->private_data = NULL;
    }
    return 0;
}
 
static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct data *p = filp->private_data;
 
    printk("Before calling the copy_from_user() function : [%p/%s]\n", p->buffer, p->buffer);
    if (copy_from_user(p->buffer, buf, count) != 0) {
        return -EFAULT;
    }
    printk("After calling the copy_from_user() function : [%p/%s]\n", p->buffer, p->buffer);
    return count;
}
 
static ssize_t chardev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct data *p = filp->private_data;
     
    if(count > BUF_LEN) {
        count = BUF_LEN;
    }
 
    if (copy_to_user(buf, p->buffer, count) != 0) {
        return -EFAULT;
    }
 
    return count;
}
 
module_init(init_chardev);
module_exit(exit_chardev);
