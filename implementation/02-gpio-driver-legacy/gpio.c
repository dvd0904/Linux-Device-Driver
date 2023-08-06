#include <linux/module.h> /* This module defines functions such as module_init/module_exit */
#include <linux/io.h>     /* This module defines functions such as ioremap/iounmap */
#include "gpio.h"         /* Gpio modules */

#define DRIVER_AUTHOR "Nguyen Dinh Dai"
#define DRIVER_DESC "Turn on and off led"

uint32_t __iomem *gpio0_start_addr;

static int __init gpio_init(void)
{
    gpio0_start_addr = ioremap(GPIO0_ADDR_BASE, GPIO0_ADDR_SIZE);

    if (!gpio0_start_addr)
        return -ENOMEM;

    /* Set gpio0_30 as output mode and turn on */
    *(gpio0_start_addr + GPIO_OE_OFFSET / 4) &= ~GPIO0_30;
    *(gpio0_start_addr + GPIO_SETDATAOUT_OFFSET / 4) |= GPIO0_30;

    printk(KERN_INFO "GPIO_START_ADDRESS1: %p", gpio0_start_addr);
    printk(KERN_INFO "GPIO_OE_ADDRESS0: %p", gpio0_start_addr + GPIO_OE_OFFSET / 4);
    printk(KERN_INFO "GPIO_OE_ADDRESS1: %p", gpio0_start_addr + GPIO_OE_OFFSET);

    pr_info("Hello! Initizliaze GPIO successfully!\n");
    return 0;
}

static void __exit gpio_exit(void)
{
    *(gpio0_start_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= GPIO0_30;
    iounmap(gpio0_start_addr);

    pr_info("Good bye my fen !!!\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("1.0");