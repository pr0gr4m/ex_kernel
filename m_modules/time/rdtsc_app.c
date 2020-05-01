#include <stdio.h>

#ifdef CONFIG_X86_64
#define DECLARE_ARGS(val, low, high)	unsigned long low, high
#define EAX_EDX_VAL(val, low, high)		((low) | (high) << 32)
#define EAX_EDX_RET(val, low, high)		"=a" (low), "=d" (high)
#else
#define DECLARE_ARGS(val, low, high)	unsigned long long val
#define EAX_EDX_VAL(val, low, high)		(val)
#define EAX_EDX_RET(val, low, high)		"=A" (val)
#endif

int main(void)
{
	unsigned long long tsc = 0;
	DECLARE_ARGS(val, low, high);
	__asm__ __volatile__("rdtsc" : EAX_EDX_RET(val, low, high));
	tsc = EAX_EDX_VAL(val, low, high);
	printf("TSC: %llu\n", tsc);
	return 0;
}
