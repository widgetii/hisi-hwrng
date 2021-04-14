ifneq ($(KERNELRELEASE),)
obj-m := hello.o
else
KDIR := $(HOME)/projects/cameras/sdk/Hi3516CV300_SDK_V1.0.3.0/osdrv/opensource/kernel/linux-3.18.20/

all:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	-rm *.ko *.o

endif
