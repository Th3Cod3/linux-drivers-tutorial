# Video 1 - Hello World LKM (Loadable Kernel Module)

To upload a module in Linux you have to use the `insmod` command. This command is used to insert a module into the Linux kernel. The module is a file with the extension `.ko` that contains the code that will be added to the kernel.

To remove a module from the kernel you have to use the `rmmod` command.

```bash
insmod module.ko
rmmod module.ko
```

In this case we use `printk` to print a message in the kernel log. To see the message we can use the `dmesg` command.

```bash
dmesg
```

Example of the output:

```log
[...] hello: loading out-of-tree module taints kernel.
[...] Hello, world!
[...] Goodbye, cruel world!
```

As we can see on each line we have the uptime timestamp `[...]` and the message is logged. In this case we have a warning from the kernel self. where it says that we are loading an out-of-tree module. After that, we have the messages that we print in the module.


