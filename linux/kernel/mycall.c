#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

SYSCALL_DEFINE3(mycall, int, a, int, b, int *, to_user)
{
	int sum = 0;
	printk("[Kernel Message] a = %d, b = %d\n", a, b);
	sum = a + b;
	put_user(sum, to_user);
	return 21;
}

SYSCALL_DEFINE1(get_cpu_info, struct cpu_info *, info)
{
	struct cpu_info src;
	sprintf(src.vendor_id, "%s", boot_cpu_data.x86_vendor_id);
	sprintf(src.model_id, "%s", boot_cpu_data.x86_model_id);
	src.cache_size = boot_cpu_data.x86_cache_size;

	if (!access_ok(info, sizeof(struct cpu_info)))
		goto error;
	else
		__copy_to_user(info, &src, sizeof(struct cpu_info));

	return 0;

error:
	return -1;
}
