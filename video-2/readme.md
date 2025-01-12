# Video 2 - Device Numbers and Files

In Linux to access a device, you can do it through a file special file call device file. Those files can be found in the `/dev` directory. Each file in the `/dev` directory is a device file. When you list the files in the `/dev` directory with `ls -l`, you will see that the first character of the file is a `c` or a `b`. The `c` stands for character device and the `b` stands for block device. You well see a number separated by comma, this is the major and minor number of the device. The major number is the number of the driver that controls the device and the minor number is the number of the device.

```bash
ls -l /dev
# crw-rw-rw- 1 root tty       5,   0 Jan 10 23:09 tty
# crw--w---- 1 root tty       4,   0 Jan  5 00:22 tty0
# crw--w---- 1 root tty       4,   1 Jan  5 00:22 tty1
# crw--w---- 1 root tty       4,  10 Jan  5 00:22 tty10
# brw-rw---- 1 root disk      1,   0 Jan  5 00:22 ram0
# brw-rw---- 1 root disk      1,   1 Jan  5 00:22 ram1
# ...
```

To see all the registered devices in the system, you can cat the `/proc/devices` file. This file contains all the devices that are registered in the system. The devices are separated by character and block devices. The devices are listed with the major number and the name of the device.

```bash
cat /proc/devices
# Character devices:
#   1 mem
#   4 /dev/vc/0
#   4 tty
#   4 ttyS
#   5 /dev/tty
#   5 /dev/console
#   5 /dev/ptmx
#
# Block devices:
#   1 ramdisk
#   7 loop
#   8 sd
#  65 sd
```

When you load the module, we code in `./char_driver.ko`, the module will register a device in the system. You can see the major number of the device in the `dmesg` command or in the `/proc/devices` file.

With the device register we can make a device file in the `/dev` directory. The device file is a special file that points to the device. The device file is created with the `mknod` command. The syntax of the `mknod` command is `mknod <device file> <type> <major> <minor>`. The type can be `c` for character device or `b` for block device.

```bash
mknod /dev/mydevice c 250 0
```

Now in user space, we can access the device with the device file `/dev/mydevice`. In our case, we only have implement the `open` and `close` operation, so we can only open and close the device file.

```c
// This function will call the open operation in our module
// it will print the message "Device opened" in the kernel log
int fd = open("/dev/mydevice", O_RDONLY);
if (fd == -1) {
    perror("open");
    return 1;
}

// this function will call the close operation in our module
// it will print the message "Device closed" in the kernel log
close(fd);
```

## Extra information

In the code you will see on the file_operations struct that all the operations have as first parameter a pointer to a struct file. This struct is a representation of the file that is being accessed. This way of programming is a object-oriented way of programming in C. Where the struct file is the object self, normally represented by the `this` keyword in other languages.
