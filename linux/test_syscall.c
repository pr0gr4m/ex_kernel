#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cpu_info {
	char vendor_id[16];
	char model_id[64];
	unsigned int cache_size;
};

int main(void)
{
	struct cpu_info info;
	int a = 10, b = 20, sum, ret;
	printf("[App Message] a = %d, b = %d \n", a, b);
	ret = syscall(548, a, b, &sum);
	printf("[App Message] ret = %d, sum = %d\n", ret, sum);

	printf("[App Message] call get_cpu_info\n");

	ret = syscall(549, &info);

	printf("[App Message] ret = %d\n", ret);
	printf("[App Message] Vendor ID: %s\n", info.vendor_id);
	printf("[App Message] Model ID: %s\n", info.model_id);
	printf("[App Message] Cache Size: %u\n", info.cache_size);
	
	return 0;
}
