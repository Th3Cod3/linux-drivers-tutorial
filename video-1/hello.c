#include <linux/module.h> // module_init, module_exit
#include <linux/init.h>

// This function name is arbitrary, at the and you use module_init()
// to tell the kernel to call this function
int my_init(void)
{
    printk(KERN_INFO "Hello, world!\n");
    return 0;
}

// Same as above, but this time you use module_exit()
// This function is called when the module is removed
void my_exit(void)
{
    printk(KERN_INFO "Goodbye, cruel world!\n");
}

// These two lines tell the kernel to call my_init() when the module is loaded
// and my_exit() when the module is removed
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
