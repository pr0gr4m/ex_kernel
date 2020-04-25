#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int __init init_callee(void)
{
	return 0;
}

void __exit exit_callee(void)
{
}

int add( int a, int b )
{
	printk( KERN_ALERT "[callee Module] add called...\n" );
	return a + b;
}

int sub( int a, int b )
{
	printk( KERN_ALERT "[callee Module] sub called...\n" );
	return a - b;
}

EXPORT_SYMBOL(add);
EXPORT_SYMBOL(sub);

module_init(init_callee);
module_exit(exit_callee);

MODULE_LICENSE("GPL");
