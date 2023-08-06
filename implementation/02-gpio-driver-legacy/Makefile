# Gpio Driver Legacy
BBB_KERNEL := /home/dvd/Workspace/bbb/bb-kernel/KERNEL
TOOLCHAIN := /home/dvd/Workspace/bbb/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin

PREFIX=arm-linux-gnueabi-
EXTRA_CFLAGS=-Wall
obj-m := gpio.o

all:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN)/$(PREFIX) -C $(BBB_KERNEL) M=$(shell pwd) modules
	
clean:
	make -C $(BBB_KERNEL) M=$(shell pwd) clean
