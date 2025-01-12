#include <linux/fs.h> // file_operations, register_chrdev, unregister_chrdev
#include <linux/init.h> // __init, __exit
#include <linux/module.h> // module_init, module_exit

#define MAJOR_NUMBER 90
#define DEVICE_NAME "my_device"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Module description");

// the name of the function is arbitrary, but have to comply with
// with the function signature of the file_operations struct open operation
static int driver_open_cb(struct inode *device_file, struct file *instance) {
  printk(KERN_INFO "Driver open\n");
  return 0;
}

// Same as driver_open_cb but for the release operation
static int driver_close_cb(struct inode *device_file, struct file *instance) {
  printk(KERN_INFO "Driver close\n");
  return 0;
}

// this struct is a operations table, it is used to tell the kernel which
// functions to call when a certain operation is performed on the device
// This can be seen a the methods of a object in object oriented programming
static struct file_operations fops = {
  // public: struct module *owner
  .owner = THIS_MODULE, // pointer to the module that owns the structure

  // public: int (*open)(struct inode *, struct file *)
  .open = driver_open_cb,

  // public: int (*release)(struct inode *, struct file *)
  .release = driver_close_cb
};

// usign the __init and __exit macros, the kernel improves the memory usage,
// and the code is optimized for the boot process
static int __init my_init(void) {
  int retval; // return value
  printk(KERN_INFO "Char driver loaded\n");

  // register_chrdev is a function from the kernel that registers a device
  // The device number is a 32-bit integer that is split into two parts: the 12
  // most significant bits are the major number, and the 20 least significant
  // bits are the minor number. You can list all major devices register in the system 
  // with the command `cat /proc/devices`
  //
  // If register_chrdev is successful, it returns 0 meaning that the sujested major number 
  // was accepted.
  // If the return value is positive, it means that the kernel assigned a different major number
  // If the return value is negative, it means that the registration failed.
  //
  // The positive number returned by register_chrdev is the registered major and minor number
  //
  // ───────────────────────────────────────────────────────────────────────
  // static inline int register_chrdev(unsigned int major, const char *name,
  //                                   const struct file_operations *fops)
  retval = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);

  if (retval == 0) {
    printk(KERN_INFO "Registered device with major number %d\n", MAJOR_NUMBER);
  } else if (retval > 0) {
    printk(KERN_INFO "Registered device with major number %d, %d\n", retval>>20, retval & 0xFFFFF);
  } else {
    printk(KERN_ALERT "Registering device failed with %d\n", retval);
    return retval;
  }

  return 0;
}

// same as __init, but this function is called when the module is removed
static void __exit my_exit(void) {
  // unregister_chrdev is a function from the kernel that unregisters a devices
  //
  // ───────────────────────────────────────────────────────────────────────
  // static inline void unregister_chrdev(unsigned int major, const char *name)
  unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
  printk(KERN_INFO "Goodbye, cruel world!\n");
}

// explain in video-1
module_init(my_init);
// explain in video-1
module_exit(my_exit);
