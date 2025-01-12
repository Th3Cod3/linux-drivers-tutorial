# this just informs the kernel that the module hello.o needs to be compiled
# there is a makefile in the kernel source that will compile the module
# this files just expands the kernel makefile to include the module hello.o
obj-m += hello.o

# shell uname -r will return the kernel version
# in `/lib/modules/<kernel-version>/build` there is a makefile that will compile the module
# the M=$(PWD) will tell the makefile to compile the module in the current directory
# and modules is the target to compile the module
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
