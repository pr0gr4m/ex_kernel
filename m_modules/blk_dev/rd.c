// for linux kernel 2.6

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/uaccess.h>

#define DEVICE_SECTOR_SIZE	512
#define DEVICE_SIZE			(4 * 1024 * 1024)
#define DEVICE_SECTOR_TOTAL	(DEVICE_SIZE / DEVICE_SECTOR_SIZE)

struct root_device_t {
	unsigned char *data;
	struct request_queue *queue;
	struct gendisk *gd;
} root_device;

static unsigned char *vdisk;

static int device_make_request(struct request_queue *q, struct bio *bio)
{
	struct root_device_t *root_dev;
	char *hdd_data;
	char *buffer;
	struct bio_vec *bvec;
	int i;

	if (((bio->bi_sector * DEVICE_SECTOR_SIZE) + bio->bi_size) >
			DEVICE_SIZE)
		goto fail;
	root_dev = (struct root_device_t *)bio->bi_bdev->
		bd_disk->private_data;
	hdd_data = root_dev->data + (bio->bi_sector * DEVICE_SECTOR_SIZE);

	bio_for_each_segment(bvec, bio, i)
	{
		buffer = kmap(bvec->bv_page) + bvec->bv_offset;
		switch (bio_data_dir(bio))
		{
			case READA:
			case READ:
				memcpy(buffer, hdd_data, bvec->bv_len);
				break;

			case WRITE:
				memcpy(hdd_data, buffer, bvec->bv_len);
				break;

			default:
				kunmap(bvec->bv_page);
				goto fail;
		}

		kunmap(bvec->bv_page);
		hdd_data += bvec->bv_len;
	}

	bio_endio(bio, bio->bi_size, 0);
	return 0;

fail:
	bio_io_error(bio, bio->bi_size);
	return -1;
}

static int device_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int device_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int device_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	return -EIO;
}

static struct block_device_operations bdops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_release,
	.ioctl = device_ioctl
};

static int __init device_init(void)
{
	vdisk = vmalloc(DEVICE_SIZE);
	
	register_blkdev(250, "vrd");

	root_device.data = vdisk;
	root_device.gd = alloc_disk(1);
	root_device.queue = blk_alloc_queue(GFP_KERNEL);
	blk_queue_make_request(root_device.queue, &device_make_request);

	root_device.gd->major = 250;
	root_device.gd->fops = &bdops;
	root_device.gd->queue = root_device.queue;
	root_device.gd->private_data = &vdisk;
	set_capacity(root_device.gd, DEVICE_SECTOR_TOTAL);

	add_disk(root_device.gd);
	return 0;
}

static void __exit device_exit(void)
{
	del_gendisk(root_device.gd);
	put_disk(root_device.gd);
	unregister_blkdev(250, "vrd");

	vfree(vdisk);
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
