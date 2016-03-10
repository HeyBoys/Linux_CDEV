ifneq ($(KERNELRELEASE),)
	obj-m := testscull.o
else 
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	rm -rf *.mod.o *.mod.c *.o
endif
