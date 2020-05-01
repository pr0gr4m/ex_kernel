#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

int __init delay_init(void)
{
	schedule_timeout_interruptible(10 * HZ);

	return 0;
}

void __init delay_exit(void)
{
}

module_init(delay_init);
module_exit(delay_exit);

MODULE_LICENSE("Dual BSD/GPL");
