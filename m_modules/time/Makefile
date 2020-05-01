obj-m += delay.o rdtsc.o ktime.o

TARGETS = rdtsc_app

KDIR := /lib/modules/$(shell uname -r)/build

default: ${TARGETS}
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc

%.c%:
	${CC} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
