obj-m += chardev.o ioctl.o cdev.o md5_dev.o

TARGETS = chardev_app ioctl_app md5_app

KDIR := /lib/modules/$(shell uname -r)/build

default: ${TARGETS}
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc

%.c%:
	${CC} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
