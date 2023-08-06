#include <linux/module.h> /* Define module_init(), module_exit() */
#include <linux/fs.h>

#define DRIVER_AUTHOR "Dai"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"

struct my_device
{
    /*dev_t dev_num = MKDEV(200, 0); // static allocate */
    dev_t dev_num;

} mdev;

/* Constructor */
static int __init
chdev_init(void)
{
    printk(KERN_INFO "Hello Dai\n");

    /* Dynamic allocating device number */
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "mdev_num") < 0)
    {
        pr_err("Fail to alloc chrdev regioin\n");
    }

    pr_info("Major = %d, Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));
    return 0;
}

/* Destructor */
static void __exit chdev_exit(void)
{
    unregister_chrdev_region(mdev.dev_num, 1);
    pr_info("Goodbye\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);