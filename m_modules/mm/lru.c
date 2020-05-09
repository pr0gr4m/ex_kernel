#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <linux/cpuset.h>
#include <linux/memcontrol.h>

#define prev_page(p)	list_entry((p)->lru.prev, struct page, lru)

void show_pfn(struct list_head *src)
{
	int page_count = 0;
	struct page *p = NULL;

	while (!list_empty(src))
	{
		if (++page_count > 20)
			break;

		p = lru_to_page(src);
		printk(KERN_CONT "(%lx) ", page_to_pfn(p));
		p = prev_page(p);
	}

	printk("page count: %d\n", page_count);
}

void show_list(void)
{
	struct pglist_data *current_pglist = NULL;
	struct lruvec *lruvec = NULL;
	struct mem_cgroup *memcg = NULL;
	struct mem_cgroup_per_node *mz;
	int i;

	for (i = 0; i < MAX_NUMNODES; i++) {
		if (NODE_DATA(i) == NULL)
			continue;

		current_pglist = NODE_DATA(i);

		if (current_pglist->node_present_pages == 0) {
			printk(KERN_ALERT "Node-%d does not have any pages.\n", i);
			continue;
		}

		memcg = get_mem_cgroup_from_mm(NULL);

		spin_lock_irq(&current_pglist->lru_lock);

#if 0
		lruvec = mem_cgroup_lruvec(memcg, current_pglist);
#else
		mz = mem_cgroup_nodeinfo(memcg, current_pglist->node_id);
		lruvec = &mz->lruvec;
#endif

		printk("========== LRU_ACTIVE_FILE ============\n");
		show_pfn(&lruvec->lists[LRU_ACTIVE_FILE]);
		printk("========== LRU_INACTIVE_FILE ============\n");
		show_pfn(&lruvec->lists[LRU_INACTIVE_FILE]);
		printk("========== LRU_ACTIVE_ANON ============\n");
		show_pfn(&lruvec->lists[LRU_ACTIVE_ANON]);
		printk("========== LRU_INACTIVE_ANON ============\n");
		show_pfn(&lruvec->lists[LRU_INACTIVE_ANON]);
		printk("========== LRU_UNEVICTABLE ============\n");
		show_pfn(&lruvec->lists[LRU_UNEVICTABLE]);
		
		spin_unlock_irq(&current_pglist->lru_lock);
	}
}

static int __init show_lru_init(void)
{
	if (mem_cgroup_disabled())
		printk("memcg disabled\n");
	else
		printk("memcg enabled\n");
	show_list();
	return 0;
}

static void __exit show_lru_exit(void)
{
}

module_init(show_lru_init);
module_exit(show_lru_exit);
MODULE_LICENSE("GPL");
