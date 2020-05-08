#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>

struct item {
	int number;
	struct list_head list;
};

// declare list
LIST_HEAD(my_list);

static struct item item1, item2, item3;

static int __init list_init(void)
{
	struct item *cur;

	item1.number = 10;
	item2.number = 20;
	item3.number = 30;

	INIT_LIST_HEAD(&my_list);

	list_add(&item1.list, &my_list);
	list_add(&item2.list, &my_list);
	list_add_tail(&item3.list, &my_list);
	// head -> 20 -> 10 -> 30

	printk("=========== First Phase ==========\n");
	list_for_each_entry(cur, &my_list, list) {
		printk("%d ", cur->number);
	}
	printk("\n");

	list_del(&item1.list);
	// head -> 20 -> 30

	printk("=========== Second Phase ==========\n");
	list_for_each_entry(cur, &my_list, list) {
		printk("%d ", cur->number);
	}
	printk("\n");

	return 0;
}

static void __exit list_exit(void)
{
	list_del_init(&my_list);
}

module_init(list_init);
module_exit(list_exit);
MODULE_LICENSE("GPL");
