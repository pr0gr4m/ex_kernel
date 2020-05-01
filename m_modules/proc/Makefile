obj-m += simple_proc.o list_proc.o

#TARGETS = simple_proc_app
KDIR := /lib/modules/$(shell uname -r)/build

# ${TARGETS]
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

CC := gcc

%.c%:
	${CC} -o $@ $^

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ${TARGETS}
