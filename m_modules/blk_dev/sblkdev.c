#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#ifndef SECTOR_SHIFT
#define SECTOR_SHIFT	9
#endif
#ifndef SECTOR_SIZE
#define SECTOR_SIZE		(1 << SECTOR_SHIFT)
#endif

static const char * _sblkdev_name = "sblkdev";
static const size_t _sblkdev_buffer_size = 16 * PAGE_SIZE;

typedef struct sblkdev_cmd_s {
} sblkdev_cmd_t;

typedef struct sblkdev_device_s {
	sector_t capacity;
	u8 *data;
	atomic_t open_counter;

	struct blk_mq_tag_set tag_set;
	struct request_queue *queue;
	struct gendisk *disk;
} sblkdev_device_t;

static unsigned int _sblkdev_major = 0;
static sblkdev_device_t *_sblkdev_device = NULL;

static int sblkdev_add_device(void);
static void sblkdev_remove_device(void);
static int sblkdev_add_device(void);

static int sblkdev_allocate_buffer(sblkdev_device_t *dev)
{
	dev->capacity = _sblkdev_buffer_size >> SECTOR_SHIFT;
	dev->data = kmalloc(dev->capacity << SECTOR_SHIFT, GFP_KERNEL);
	if (dev->data == NULL) {
		printk(KERN_ERR "kmalloc error");
		return -ENOMEM;
	}
	return 0;
}

static void sblkdev_free_buffer(sblkdev_device_t *dev)
{
	if (dev->data) {
		kfree(dev->data);
		dev->data = NULL;
		dev->capacity = 0;
	}
}

static int do_simple_request(struct request *rq, unsigned int *nr_bytes)
{
	int ret = 0;
	struct bio_vec bvec;
	struct req_iterator iter;
	sblkdev_device_t *dev = rq->q->queuedata;
	loff_t pos = blk_rq_pos(rq) << SECTOR_SHIFT;
	loff_t dev_size = (loff_t)(dev->capacity << SECTOR_SHIFT);

	printk("sblkdev: request start from sector %lld\n",
			blk_rq_pos(rq));

	rq_for_each_segment(bvec, rq, iter)
	{
		unsigned long b_len = bvec.bv_len;
		void *b_buf = page_address(bvec.bv_page) + bvec.bv_offset;

		if ((pos + b_len) > dev_size)
			b_len = (unsigned long)(dev_size - pos);

		if (rq_data_dir(rq))	// write
			memcpy(dev->data + pos, b_buf, b_len);
		else
			memcpy(b_buf, dev->data + pos, b_len);

		pos += b_len;
		*nr_bytes += b_len;
	}

	return ret;
}

/*
 * hctx - the state of the hardware queue
 * bd - request
 */
static blk_status_t queue_rq(struct blk_mq_hw_ctx *hctx,
		const struct blk_mq_queue_data *bd)
{
	blk_status_t status = BLK_STS_OK;
	struct request *rq = bd->rq;

	blk_mq_start_request(rq);

	{
		unsigned int nr_bytes = 0;
		if (do_simple_request(rq, &nr_bytes) != 0)
			status = BLK_STS_IOERR;
		printk("sblkdev: request process %d bytes\n", nr_bytes);

#if 1
		blk_mq_end_request(rq, status);
#else
		if (blk_update_request(rq, status, nr_bytes))
			BUG();
		__blk_mq_end_request(rq, status);
#endif
	}

	return BLK_STS_OK;
}

static int _open(struct block_device *bdev, fmode_t mode)
{
	sblkdev_device_t *dev = bdev->bd_disk->private_data;
	if (dev == NULL) {
		printk(KERN_ERR "_open error");
		return -ENXIO;
	}

	atomic_inc(&dev->open_counter);
	printk("device was opened\n");

	return 0;
}

static void _release(struct gendisk *gd, fmode_t mode)
{
	sblkdev_device_t *dev = gd->private_data;
	if (dev) {
		atomic_dec(&dev->open_counter);
		printk("device was closed\n");
	} else {
		printk(KERN_ERR "Invalid to release disk");
	}
}

static int _ioctl(struct block_device *bdev, fmode_t mode, 
		unsigned int cmd, unsigned long arg)
{
	return -ENOTTY;
}

#ifdef CONFIG_COMPAT
static int _compat_ioctl(struct block_device *blkdev, fmode_t mode, unsigned u, unsigned long l)
{
	return -ENOTTY;
}
#endif

static struct blk_mq_ops _mq_ops = {
	.queue_rq = queue_rq
};

static const struct block_device_operations _fops = {
	.owner = THIS_MODULE,
	.open = _open,
	.release = _release,
	.ioctl = _ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = _compat_ioctl
#endif
};

static int sblkdev_add_device(void)
{
	int ret = 0;
	sblkdev_device_t *dev = kzalloc(sizeof(sblkdev_device_t), GFP_KERNEL);
	if (dev == NULL) {
		printk(KERN_ERR "kzalloc error");
		return -ENOMEM;
	}
	_sblkdev_device = dev;

	do {
		ret = sblkdev_allocate_buffer(dev);
		if (ret)
			break;

#if 1
		{
			struct request_queue *queue;
			dev->tag_set.cmd_size = sizeof(sblkdev_cmd_t);
			dev->tag_set.driver_data = dev;

			queue = blk_mq_init_sq_queue(&dev->tag_set, &_mq_ops, 128,
					BLK_MQ_F_SHOULD_MERGE);		//| BLK_MQ_F_SG_MERGE);
			if (IS_ERR(queue)) {
				ret = PTR_ERR(queue);
				printk(KERN_ERR "blk_mq_init_sq_queue error");
				break;
			}
			dev->queue = queue;
		}
#else
		{		// configure tag_set
			dev->tag_set.ops = &_mq_ops;
			dev->tag_set.nr_hw_queues = 1;
			dev->tag_set.queue_depth = 128;
			dev->tag_set.numa_node = NUMA_NO_NODE;
			dev->tag_set.cmd_size = sizeof(sblkdev_cmd_t);
			dev->tag_set.flags = BLK_MQ_F_SHOULD_MERGE | BLK_MQ_F_SG_MERGE;
			dev->tag_set.driver_data = dev;

			ret = blk_mq_alloc_tag_set(&dev->tag_set);
			if (ret) {
				printk(KERN_ERR "blk_mq_alloc_tag_set error");
				break;
			}
		}
		{		// configure queue
			struct request_queue *queue = blk_mq_init_queue(&dev->tag_set);
			if (IS_ERR(queue)) {
				ret = PTR_ERR(queue);
				printk(KERN_ERR "blk_mq_init_queue error");
				break;
			}
			dev->queue = queue;
		}
#endif

		dev->queue->queuedata = dev;
		{		// configure disk
			struct gendisk *disk = alloc_disk(1);
			if (disk == NULL) {
				printk(KERN_ERR "alloc_disk error");
				ret = -ENOMEM;
				break;
			}

			disk->flags |= GENHD_FL_NO_PART_SCAN;	// only one partition
			disk->flags |= GENHD_FL_REMOVABLE;

			disk->major = _sblkdev_major;
			disk->first_minor = 0;
			disk->fops = &_fops;
			disk->private_data = dev;
			disk->queue = dev->queue;
			sprintf(disk->disk_name, "sblkdev%d", 0);
			set_capacity(disk, dev->capacity);
			dev->disk = disk;
			add_disk(disk);
		}

		printk("sblkdev: simple block device was created\n");
	} while (false);

	if (ret) {
		sblkdev_remove_device();
		printk(KERN_ERR "sblkdev: Failed to add block devce \n");
	}

	return ret;
}

static void sblkdev_remove_device(void)
{
	sblkdev_device_t *dev = _sblkdev_device;
	if (dev) {
		if (dev->disk)
			del_gendisk(dev->disk);
		if (dev->queue) {
			blk_cleanup_queue(dev->queue);
			dev->queue = NULL;
		}
		if (dev->tag_set.tags)
			blk_mq_free_tag_set(&dev->tag_set);
		if (dev->disk) {
			put_disk(dev->disk);
			dev->disk = NULL;
		}

		sblkdev_free_buffer(dev);
		kfree(dev);
		_sblkdev_device = NULL;

		printk("sblkdev: simple block device was removed\n");
	}
}


static int __init sblkdev_init(void)
{
	int ret = 0;
	_sblkdev_major = register_blkdev(_sblkdev_major, _sblkdev_name);
	if (_sblkdev_major <= 0) {
		printk(KERN_ERR "register_blkdev error");
		return -EBUSY;
	}

	ret = sblkdev_add_device();
	if (ret)
		unregister_blkdev(_sblkdev_major, _sblkdev_name);
	return ret;
}

static void __exit sblkdev_exit(void)
{
	sblkdev_remove_device();
	if (_sblkdev_major > 0)
		unregister_blkdev(_sblkdev_major, _sblkdev_name);
}

module_init(sblkdev_init);
module_exit(sblkdev_exit);
