#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE( "GPL" );

int __init init_hello(void)
{
	printk( KERN_ALERT "[Module Message] Hello, Module.\n" );
	return 0;
}

void __exit exit_hello(void)
{
	printk( KERN_ALERT "[Module Message] Bye, Module.\n" );
}

module_init( init_hello );
module_exit( exit_hello );

