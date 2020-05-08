obj-m += list.o

TARGETS = linked

KDIR := /lib/modules/$(shell uname -r)/build

default: ${TARGETS}
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc
CFLGAS := -Wall

%.c%:
	${CC} ${CFLAGS} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
