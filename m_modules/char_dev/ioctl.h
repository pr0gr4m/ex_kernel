#ifndef __IOCTL_H__
#define __IOCTL_H__

#include <linux/ioctl.h>

struct ioctl_info {
	unsigned long size;
	char buf[128];
};

#define IOCTL_MAGIC		'G'
#define IOCTL_READ		_IOR(IOCTL_MAGIC, 0, struct ioctl_info)
#define IOCTL_WRITE		_IOW(IOCTL_MAGIC, 1, struct ioctl_info)
#define IOCTL_STATUS	_IO(IOCTL_MAGIC, 2)
#define IOCTL_RW		_IOWR(IOCTL_MAGIC, 3, struct ioctl_info)
#define IOCTL_NR		4

#endif
