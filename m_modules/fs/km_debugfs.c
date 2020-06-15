#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/memblock.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/cpu.h>
#include <linux/delay.h>
#include <asm/setup.h>
#include <linux/input.h>
#include <linux/debugfs.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/memory.h>

uint32_t km_debug_state = 0x1000;
static struct dentry *km_kernel_debugfs_root;

static int km_kernel_debug_stat_get(void *data, u64 *val)
{
	printk("[%s][L:%d][val:%d]\n", __func__, __LINE__, km_debug_state);
	*val = km_debug_state;
	return 0;
}

static int km_kernel_debug_stat_set(void *data, u64 val)
{
	km_debug_state = (uint32_t)val;
	printk("[km] [%s][L:%d], [km_debug_state:%lu], [value:%lu]\n",
			__func__, __LINE__, (long unsigned int)km_debug_state, (long unsigned int)val);
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(km_kernel_debug_stat_fops, km_kernel_debug_stat_get,
		km_kernel_debug_stat_set, "%llu\n");

static int km_kernel_debugfs_driver_probe(struct platform_device *pdev)
{
	printk("[%s][L:%d]\n", __func__, __LINE__);
	return 0;
}

static struct platform_driver km_kernel_debugfs_driver = {
	.probe = km_kernel_debugfs_driver_probe,
	.driver = {
		.owner = THIS_MODULE,
		.name = "km_debug",
	},
};

static int __init km_kernel_debugfs_init(void)
{
	printk("[%s][L:%d]\n", __func__, __LINE__);
	
	km_kernel_debugfs_root = debugfs_create_dir("km_debug", NULL);
	debugfs_create_file("val", S_IRUGO, km_kernel_debugfs_root, NULL, &km_kernel_debug_stat_fops);
	return platform_driver_register(&km_kernel_debugfs_driver);
}

late_initcall(km_kernel_debugfs_init);

MODULE_DESCRIPTION("raspberrypi debug interface driver");
MODULE_LICENSE("GPL");
