#ifndef __LIST_PROC_H__
#define __LIST_PROC_H__

#include <linux/mutex.h>
#include <linux/list.h>

//#define USE_SINGLE_OPEN

struct m_info {
	int a;
	int b;
	struct list_head list;
};

extern struct mutex m_lock;
extern struct list_head m_list;

#endif
