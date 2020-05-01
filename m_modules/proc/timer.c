#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#include <linux/seq_file.h>
#include <asm/processor.h>
#include <asm/tsc.h>

static int proc_show(struct seq_file *m, void *v)
{	
	unsigned long m_jiffy = get_jiffies_64();

	unsigned long long cycles = 0;

	struct timespec64 m_timespec;

	seq_printf(m, "[Message] HZ: %d\n", HZ);
	seq_printf(m, "[Message] jiffies: %lu\n", m_jiffy);
	seq_printf(m, "[Message] jiffies after ls: %lu\n", m_jiffy + HZ);

	seq_printf(m, "[Message] uptime in min: %lu\n",
			m_jiffy / HZ / 60);
	seq_printf(m, "[Message] uptime in sec: %lu\n",
			m_jiffy / HZ);

	seq_printf(m, "[Message] ktime: %llu\n", ktime_get());
	ktime_get_ts64(&m_timespec);
	seq_printf(m, "[Message] get_ts64: %20llu.%09lu\n",
			m_timespec.tv_sec, m_timespec.tv_nsec);
	ktime_get_raw_ts64(&m_timespec);
	seq_printf(m, "[Message] get_raw_ts64: %20llu.%09lu\n",
			m_timespec.tv_sec, m_timespec.tv_nsec);
	ktime_get_real_ts64(&m_timespec);
	seq_printf(m, "[Message] get_real: %20llu.%09lu\n",
			m_timespec.tv_sec, m_timespec.tv_nsec);
	ktime_get_coarse_ts64(&m_timespec);
	seq_printf(m, "[Message] get_coarse: %20llu.%09lu\n",
			m_timespec.tv_sec, m_timespec.tv_nsec);
	ktime_get_coarse_real_ts64(&m_timespec);
	seq_printf(m, "[Message] get_coarse_real: %20llu.%09lu\n",
			m_timespec.tv_sec, m_timespec.tv_nsec);

	cycles = get_cycles();
	seq_printf(m, "[Message] cycles: %llu\n", cycles);
	seq_printf(m, "[Message] cpu_khz: %u\n", cpu_khz);

	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
	.proc_open = proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};

static int __init timer_init(void)
{
	proc_create("timer", 0, NULL, &proc_fops);
	return 0;
}

static void __exit timer_exit(void)
{
	remove_proc_entry("timer", NULL);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
