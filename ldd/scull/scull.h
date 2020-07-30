#ifndef _SCULL_H_
#define _SCULL_H_

#ifndef SCULL_MAJOR
#define SCULL_MAJOR	0
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS	4
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS	4
#endif

#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM	4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET	1000
#endif

#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER	4000
#endif

/*
 * scull quantum sets
 */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;	/* Pointer to first quantum set */
	int quantum;			/* the current quantum size */
	int qset;			/* the current array size */
	unsigned long size;		/* amount of data stored here */
	unsigned int access_key;	/* used by sculluid and scullpriv */
	struct mutex lock;		/* mutual exclusion semaphore */
	struct cdev cdev;		/* char device structure */
};

/*
 * Split minors in two parts
 */
#define TYPE(minor)	(((minor) >> 4) & 0xF)	/* high nibble */
#define NUM(minor)	((minor) & 0xF)		/* low nibble */

/*
 * The different configurable parameters
 */
extern int scull_major;
extern int scull_nr_devs;
extern int scull_quantum;
extern int scull_qset;
extern int scull_p_buffer;

/*
 * Prototypes for shared functions
 */
int scull_trim(struct scull_dev *dev);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos);
loff_t scull_llseek(struct file *filp, loff_t off, int whence);
long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#endif
