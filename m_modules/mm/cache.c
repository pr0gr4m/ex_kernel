#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

static struct kmem_cache *my_cachep;

struct my {
	void *data;
};

static struct my *p;

static int __init cache_init(void)
{
	my_cachep = kmem_cache_create(
			"my_cache",				// cache name
			sizeof(struct my),		// slab object size
			0,						// no align
			SLAB_HWCACHE_ALIGN,		// hardware align
			NULL					// no constructor
			);

	if (my_cachep != NULL) {
		printk(KERN_INFO "kmem_cache_create success\n");
		p = (struct my *)kmem_cache_alloc(my_cachep, GFP_KERNEL);
	}
	return 0;
}

static void __exit cache_exit(void)
{
	kmem_cache_free(my_cachep, p);
	kmem_cache_destroy(my_cachep);
}

module_init(cache_init);
module_exit(cache_exit);
MODULE_LICENSE("GPL");
