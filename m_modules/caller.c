#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int add( int, int );
int sub( int, int );

int __init init_caller(void)
{
	printk( KERN_ALERT "[caller Module] call add(), sub()\n" );
	printk( KERN_ALERT "[caller Module] add: %d\n", add( 3, 2 ) );
	printk( KERN_ALERT "[caller Module] sub: %d\n", sub( 3, 2 ) );
	return 0;
}

void __exit exit_caller(void)
{
}

module_init(init_caller);
module_exit(exit_caller);

MODULE_LICENSE("GPL");
