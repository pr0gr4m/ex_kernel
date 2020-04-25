#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

int a;
module_param( a, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

int __init init_param(void)
{
	printk( KERN_ALERT "[Module Message] parameter a = %d\n", a );
	return 0;
}

void __exit exit_param(void) {}

module_init( init_param );
module_exit( exit_param );

MODULE_LICENSE( "GPL" );
