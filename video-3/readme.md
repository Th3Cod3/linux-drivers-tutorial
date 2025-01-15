# Video 3 - Read and Write Operations

In this tutorial we use the "new" API to register a device number, create a class and device, and implement the read and write operations.

## Device Number

The device number is a unique identifier for a device. It is used to create a device file in `/dev/` and to register the device with the kernel.

The new API to register a device number is `alloc_chrdev_region`. The function prototype is:

```c
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
```

- `dev_t`: is a 32-bit integer that contains the major and minor number of the device
  - `*dev`: pointer to the device number
- `baseminor`: the first minor number to allocate
- `count`: the maximum number of minor numbers to allocate
- `name`: the name of the device

The function returns 0 on success and a negative error code on failure.

## Class

The class is a way to group devices. All the devices in the same class will be listed in the same directory in `/sys/class/<class_name>/<device_name>`. The class is created with the function `class_create`. The function prototype is:

```c
// This function is used in the video, but this prototype has changed in the kernel
struct class *class_create(struct module *owner, const char *name);
// This is the new prototype
struct class *class_create(const char *name);
```

- `name`: the name of the class, it will be the directory name in `/sys/class/`.

The function returns a pointer to the class on success and a `NULL` pointer on failure.

## Interacting with the device

After loading the module, the device file will be created in `/dev/<device_name>`. This time we have implement the read and write operations. So, if a user space program writes to the device file, the kernel will call the `write` function. If a user space program reads from the device file, the kernel will call the `read` function.

That is also the reason we user the `copy_to_user` and `copy_from_user` functions. so we can interact with the user space data. So we can interact with the device file as a regular file. From a C program or from the terminal.

```bash
echo "Hello World" > /dev/<device_name>
head -n 1 /dev/<device_name>
```
