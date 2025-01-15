#include <linux/fs.h> // file_operations
#include <linux/init.h> // __init, __exit
#include <linux/module.h> // module_init, module_exit
#include <linux/kernel.h> // printk
#include <linux/cdev.h> // cdev_init, cdev_add, cdev_del
#include <linux/uaccess.h> // copy_to_user, copy_from_user

#define MAJOR_NUMBER 5
#define DEVICE_NAME "my_device"
#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyDummyClass"
#define BUFFER_SIZE 255

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Module description");

// module data
static char kernel_buffer[BUFFER_SIZE];
// on write we will move the pointer to the last position written
static size_t buffer_pointer;

// device data
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

// This function is to assign to the file_operations struct to read from the device
static ssize_t driver_read_cb(struct file *instance, char *user_buffer, size_t count, loff_t *offset) {
  int to_copy;
  int not_copied;
  int delta;

  to_copy = min(count, buffer_pointer);

  // copy_to_user safely copies data from kernel space to user space
  //
  // ────────────────────────────────────────────────────────────────────
  // static inline unsigned long copy_to_user(void *to, const void *from,
  //                                          unsigned long n)
  not_copied = copy_to_user(user_buffer, kernel_buffer, to_copy);

  delta = to_copy - not_copied;

  return delta;
}

static ssize_t driver_write_cb(struct file *instance, const char *user_buffer, size_t count, loff_t *offset) {
  int to_copy;
  int not_copied;
  int delta;

  to_copy = min(count, sizeof(kernel_buffer));

  // copy_from_user safely copies data from user space to kernel space
  //
  // ────────────────────────────────────────────────────────────────────
  // static inline unsigned long copy_from_user(void *to, const void *from,
  //                                            unsigned long n)
  not_copied = copy_from_user(kernel_buffer, user_buffer, to_copy);
  buffer_pointer = to_copy;

  delta = to_copy - not_copied;

  return delta;
}

// explain in video-2
static int driver_open_cb(struct inode *device_file, struct file *instance) {
  printk(KERN_INFO "Driver open\n");
  return 0;
}

// explain in video-2
static int driver_close_cb(struct inode *device_file, struct file *instance) {
  printk(KERN_INFO "Driver close\n");
  return 0;
}

// explain in video-2
static struct file_operations fops = {
  // explain in video-2
  .owner = THIS_MODULE,

  // explain in video-2
  .open = driver_open_cb,

  // explain in video-2
  .release = driver_close_cb,

  // public: ssize_t (*read)(struct file *, char *, size_t, loff_t *)
  .read = driver_read_cb,

  // public: ssize_t (*write)(struct file *, const char *, size_t, loff_t *)
  .write = driver_write_cb
};

// explain in video-2
static int __init my_init(void) {
  printk(KERN_INFO "Kernel Module INIT!\n");

  // This function registers the device with the kernel and makes it available
  // in the /proc/devices file as DRIVER_NAME
  // ───────────────────────────────────────────────────────────────────
  // extern int alloc_chrdev_region(dev_t *, unsigned int, unsigned int,
  //                                const char *)
  if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME)) {
    printk(KERN_ERR "Device Nr. could not be allocated!\n");

    return -1;
  }
  printk(KERN_INFO "read_write: Major number: %d; Minor number: %d\n", MAJOR(my_device_nr), MINOR(my_device_nr));

  // This function creates a struct class structure and registers it with the
  // kernel so the class can be listed in /sys/class/<DRIVER_CLASS>/
  // ────────────────────────────────────────────
  // struct class *class_create(const char *name)
  if((my_class = class_create(DRIVER_CLASS)) == NULL) {
    printk(KERN_ERR "Device class can not be created!\n");

    goto ClassError;
  }

  // This function registers the device with the kernel under the class and device number
  // and can be listed in /sys/class/<DRIVER_CLASS>/<DRIVER_NAME> and /dev/<DRIVER_NAME>
  // ─────────────────────────────────────────────────────────────────────────────
  // - Easy functions for dynamically creating devices on the fly
  // struct device *device_create(const struct class *cls, struct device *parent,
  //                              dev_t devt, void *drvdata, const char *fmt, ...)
  if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
    printk(KERN_ERR "Device can not be created!\n");

    goto DeviceError;
  }

  // This function initializes a cdev structure, and assigns a file_operations
  // ─────────────────────────────────────────────────────────────
  // void cdev_init(struct cdev *, const struct file_operations *)
  cdev_init(&my_device, &fops);

  // make sure that the /dev/DRIVER_NAME is functional
  // ────────────────────────────────────────────────
  // int cdev_add(struct cdev *, dev_t, unsigned int)
  if(cdev_add(&my_device, my_device_nr, 1) == -1) {
    printk(KERN_ERR "Device can not be added!\n");

    goto AddError;
  }

  return 0;

AddError:
  device_destroy(my_class, my_device_nr);
DeviceError:
  class_destroy(my_class);
ClassError:
  unregister_chrdev_region(my_device_nr, 1);
  return -1;

}

// explain in video-2
static void __exit my_exit(void) {
  // all this functions just clean up the resources that we have allocated
  // so no explanation is needed
  cdev_del(&my_device);
  device_destroy(my_class, my_device_nr);
  class_destroy(my_class);
  unregister_chrdev_region(my_device_nr, 1);
  printk(KERN_INFO "Goodbye, cruel world!\n");
}

// explain in video-1
module_init(my_init);
// explain in video-1
module_exit(my_exit);
