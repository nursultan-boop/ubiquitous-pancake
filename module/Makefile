# Name of the kernel module
obj-m += wifi.o

# Path to the kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Flags to pass to the compiler
EXTRA_CFLAGS := -I$(src)

# Build rule for the kernel module
all:
	make -C $(KDIR) M=$(shell pwd) modules

# Clean rule
clean:
	make -C $(KDIR) M=$(shell pwd) clean
