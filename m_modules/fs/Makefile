obj-m += sfs.o km_debugfs.o

KDIR := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc

%.c%:
	${CC} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
