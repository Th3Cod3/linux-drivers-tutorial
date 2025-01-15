#include <linux/fs.h> // file_operations
#include <linux/init.h> // __init, __exit
#include <linux/module.h> // module_init, module_exit
#include <linux/kernel.h> // printk
#include <linux/cdev.h> // cdev_init, cdev_add, cdev_del
#include <linux/uaccess.h> // copy_to_user, copy_from_user
#include <linux/gpio.h> // gpio_request, gpio_direction_output, gpio_set_value, gpio_free

#define MAJOR_NUMBER 5
#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyDummyClass"
#define GPIO_LED_PIN 17
#define GPIO_BTN_PIN 18

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Module description");

// explain in video-3
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

// explain in video-3
static ssize_t driver_read_cb(struct file *instance, char *user_buffer, size_t count, loff_t *offset) {
  int to_copy;
  int not_copied;
  int delta;
  // The size is 3 to fit the value and an eol and a null terminator
  char tmp[3] = " \n";

  to_copy = min(count, sizeof(tmp));

  printk(KERN_INFO "Value of BTN: %d\n", gpio_get_value(GPIO_BTN_PIN));
  tmp[0] = gpio_get_value(GPIO_BTN_PIN) + '0';

  // explain in video-3
  not_copied = copy_to_user(user_buffer, &tmp, to_copy);

  delta = to_copy - not_copied;

  return delta;
}

static ssize_t driver_write_cb(struct file *instance, const char *user_buffer, size_t count, loff_t *offset) {
  int to_copy;
  int not_copied;
  int delta;
  char value;

  to_copy = min(count, sizeof(value));

  // explain in video-3
  not_copied = copy_from_user(&value, user_buffer, to_copy);

  switch (value) {
    case '0':
      gpio_set_value(GPIO_LED_PIN, 0);
      break;
    case '1':
      gpio_set_value(GPIO_LED_PIN, 1);
      break;
    default:
      printk(KERN_ERR "Invalid input value: %c\n", value);
      break;
  }

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

  // explain in video-3
  .read = driver_read_cb,

  // explain in video-3
  .write = driver_write_cb
};

// explain in video-2
static int __init my_init(void) {
  printk(KERN_INFO "Kernel Module INIT!\n");

  // explain in video-3
  if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME)) {
    printk(KERN_ERR "Device Nr. could not be allocated!\n");

    return -1;
  }
  printk(KERN_INFO "read_write: Major number: %d; Minor number: %d\n", MAJOR(my_device_nr), MINOR(my_device_nr));

  // explain in video-3
  if((my_class = class_create(DRIVER_CLASS)) == NULL) {
    printk(KERN_ERR "Device class can not be created!\n");

    goto ClassError;
  }

  // explain in video-3
  if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
    printk(KERN_ERR "Device can not be created!\n");

    goto DeviceError;
  }

  // explain in video-3
  cdev_init(&my_device, &fops);

  // explain in video-3
  if(cdev_add(&my_device, my_device_nr, 1) == -1) {
    printk(KERN_ERR "Device can not be added!\n");

    goto AddError;
  }

  // ────────────────────────────────────────────────────────
  // int gpio_request(unsigned int gpio, const char *label)
  if (gpio_request(GPIO_LED_PIN, "my-gpio-led") < 0) {
    printk(KERN_ERR "GPIO request failed for LED at pin %d\n", GPIO_LED_PIN);

    goto GpioLedRequestError;
  }

  // ─────────────────────────────────────────────────────────────────────
  // static inline int gpio_direction_output(unsigned int gpio, int value)
  if (gpio_direction_output(GPIO_LED_PIN, 0) < 0) {
    printk(KERN_ERR "GPIO direction output failed for LED at pin %d\n", GPIO_LED_PIN);

    goto GpioLedError;
  }

  if (gpio_request(GPIO_BTN_PIN, "my-gpio-btn") < 0) {
    printk(KERN_ERR "GPIO request failed for BTN at pin %d\n", GPIO_BTN_PIN);

    goto GpioLedError;
  }

  // ─────────────────────────────────────────────────────────
  // static inline int gpio_direction_input(unsigned int gpio)
  if (gpio_direction_input(GPIO_BTN_PIN) < 0) {
    printk(KERN_ERR "GPIO direction input failed for BTN at pin %d\n", GPIO_BTN_PIN);

    goto GpioBtnError;
  }

  return 0;

GpioBtnError:
  gpio_free(GPIO_BTN_PIN);
GpioLedError:
  gpio_free(GPIO_LED_PIN);
GpioLedRequestError:
  cdev_del(&my_device);
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
  // explain in video-3
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
