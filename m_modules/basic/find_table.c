#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <linux/kobject.h>

#define START_CHECK	0xffffffff81000000
#define END_CHECK	0xffffffffa2000000

static void **sys_call_table;

static void **find_system_call_table(void)
{
	unsigned long ptr;
	unsigned long *p;

	for (ptr = (unsigned long)START_CHECK;
			ptr < (unsigned long)END_CHECK; ptr += sizeof(void*))
	{
		p = (unsigned long *)ptr;
		if (p[__NR_close] == (unsigned long)ksys_close)
			return (void **)p;
	}
	return NULL;
}

int __init sct_init(void)
{
	sys_call_table = find_system_call_table();
	if (sys_call_table != NULL)
		printk(KERN_ALERT "sys_call_table: 0x%p\n", (void*)sys_call_table);
	else
		printk(KERN_ALERT "Failed to find sys_call_table\n");
	return 0;
}

void __exit sct_exit(void) {}

module_init(sct_init);
module_exit(sct_exit);

MODULE_LICENSE("GPL");
