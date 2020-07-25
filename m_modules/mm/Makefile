obj-m += cache.o mmap_dev.o show_mem.o lru.o slab_ex.o

TARGETS = mmap_app

KDIR := /lib/modules/$(shell uname -r)/build

default: ${TARGETS}
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc

%.c%:
	${CC} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
