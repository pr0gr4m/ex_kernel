#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

int b[] = {1, 2, 3, 4, 5};
module_param_array(b, int, NULL, 0);

int __init init_param(void)
{
	printk(KERN_ALERT "[Module Message] %d %d %d %d %d\n",
			b[0], b[1], b[2], b[3], b[4]);
	return 0;
}

void __exit exit_param(void) {}

module_init(init_param);
module_exit(exit_param);

MODULE_LICENSE("GPL");
