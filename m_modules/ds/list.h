#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
	struct list_head *next, *prev;
};

struct item {
	int number;
	struct list_head list;
};

struct list_head head = { &head, &head };

#ifndef container_of
#define container_of(ptr, type, member) ({ \
		const typeof( ((type *)0)->member ) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member)); })
#endif

#ifndef list_entry
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)
#endif

#define list_to_item(_head)	(list_entry((_head)->prev, \
			struct item, list))
#define prev_item(p)	list_entry((p)->list.prev, struct item, list)

#define for_each_item(p)	\
	for (p = list_to_item(&head); \
			&p->list != &head; \
			p = prev_item(p))

static inline void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new,
		struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void __list_del(struct list_head *prev,
		struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

#endif
