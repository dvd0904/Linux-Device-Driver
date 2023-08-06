#include <linux/module.h> /* Define module_init(), module_exit() */

#define DRIVER_AUTHOR "dvd"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"

/* Constructor */
static int __init hello_world_init(void)
{
    pr_info("Hello world kernel module\n");
    return 0;
}

/* Destructor */
static void __exit hello_world_exit(void)
{
    pr_info("Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);