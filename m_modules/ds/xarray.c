#include <linux/xarray.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static DEFINE_XARRAY(array);

static void xa_user_dump(const struct xarray *xa)
{
	void *entry = xa->xa_head;
	pr_info("xarray: %px head %px flags %x marks %d %d %d\n", xa, entry,
			xa->xa_flags, xa_marked(xa, XA_MARK_0),
			xa_marked(xa, XA_MARK_1), xa_marked(xa, XA_MARK_2));
}

static int __init xarray_user_test(void)
{
	unsigned long i;
	void *ret;

	struct item {
		unsigned long index;
		unsigned int order;
	};
	struct item *item;

	pr_info("xarray_user_test() starting...\n");

	item = kmalloc(sizeof(*item), GFP_KERNEL);
	item->index = 0;
	item->order = 100;
	pr_info("[0] item = %p\n", item);
	ret = xa_store(&array, 0, (void *)item, GFP_KERNEL);

	for (i = 1; i <= 10; i++) {
		item = kmalloc(sizeof(*item), GFP_KERNEL);
		pr_info("[%d] item = %p, ", i, item);
		item->index = i;
		item->order = i + 100;
		ret = xa_store(&array, i, (void *)item, GFP_KERNEL);
		pr_info("ret = %p\n", ret);
		xa_user_dump(&array);
	}

	ret = xa_load(&array, 0);
	pr_info("load ret = %p, %lu, %u\n", ret,
			((struct item *)ret)->index, ((struct item *)ret)->order);
	ret = xa_load(&array, 8);
	pr_info("load ret = %p, %lu, %u\n", ret,
			((struct item *)ret)->index, ((struct item *)ret)->order);

	ret = xa_erase(&array, 4);
	pr_info("erase ret = %p, %lu, %u\n", ret,
			((struct item *)ret)->index, ((struct item *)ret)->order);
	ret = xa_erase(&array, 7);
	pr_info("erase ret = %p, %lu, %u\n", ret,
			((struct item *)ret)->index, ((struct item *)ret)->order);

	i = 0;
	ret = xa_find(&array, &i, ULONG_MAX, XA_PRESENT);
	pr_info("find ret = %p, %lu, %u\n", ret,
			((struct item *)ret)->index, ((struct item *)ret)->order);

	xa_for_each(&array, i, ret) {
		pr_info("each ret = %p, %lu, %u\n", ret,
				((struct item *)ret)->index, ((struct item *)ret)->order);
	}

	xa_destroy(&array);
	pr_info("xarray_user_test() end.\n");
	return 0;
}

static void __exit xarray_exit(void)
{
	pr_info("xarray_exit() end.\n");
}

module_init(xarray_user_test);
module_exit(xarray_exit);
MODULE_LICENSE("GPL");
