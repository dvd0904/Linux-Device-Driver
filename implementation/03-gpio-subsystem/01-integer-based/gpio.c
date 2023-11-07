#include <linux/module.h>   /* Defines funtions such as module_init/module_exit */
#include <linux/gpio.h>     /* Defines functions such as gpio_request/gpio_free */

#define DRIVER_AUTHOR   "Dai dinhdai0904@gmail.com"
#define DRIVER_DESC     "GPIO Subsystem"

#define GPIO1_16    48      /* 32*1 + 16*/
#define LOW         0   
#define HIGH        1


/* Constructor */
static int __init gpio_init(void)
{
    gpio_request(GPIO1_16, "GPIO1_16");         /* This function is used to request control of a specific GPIO pin from the system */
    gpio_direction_output(GPIO1_16, LOW);
    gpio_set_value(GPIO1_16, HIGH);

    pr_info("Hello! GPIO1_16 status: %d.\n", gpio_get_value(GPIO1_16));

    return 0;
}

static void __exit gpio_exit(void)
{
    gpio_set_value(GPIO1_16, LOW);
    gpio_free(GPIO1_16);

    pr_info("Good bye!\n");

}


module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION("1.0"); 