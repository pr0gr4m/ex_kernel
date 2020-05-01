#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/msr.h>

int __init m_tsc_init(void)
{
	printk(KERN_INFO "[Message] TSC: %llu\n", rdtsc());
	return 0;
}

void __exit m_tsc_exit(void)
{}

module_init(m_tsc_init);
module_exit(m_tsc_exit);

MODULE_LICENSE("Dual BSD/GPL");
