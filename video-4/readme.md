# Video 4 - GPIO driver

During this tutorial I was curious about the implentation of the GPIO driver API. I have an vague idea that the hardware is described in the device tree and the kernel loads the dtb into memory and uses it to interact with the hardware. But I don't know how the kernel interacts with dtb to map the GPIO pins to the kernel. I have also developed bear metal applications in AVR and STM32 microcontrollers. So I have an idea how is a GPIO peripheral works in a microcontroller.

So I decided to take a look at the source code and deep dive into the implementation of `gpio_request`. Then I saw that the function `gpio_request` use `gpio_to_desc` and searching for `gpio_to_desc` I found the documention and understood how the kernel documents the GPIO pins. Then I start reading the documentation and realease that the videos are using the legacy way to interact with GPIO pins and that this way is deprecated.

In the documentation they suggest to see the example from some [Subsystem driver using GPIO](https://www.kernel.org/doc/html/v6.9/driver-api/gpio/drivers-on-gpio.html).

So I'll see the video to understand the the topic and then I'll implement the GPIO driver using the new API.

