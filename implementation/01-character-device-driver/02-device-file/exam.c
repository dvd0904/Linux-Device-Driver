#include <linux/module.h> /* Define module_init(), module_exit() */
#include <linux/fs.h>     /* Allocate major and minor number */
#include <linux/device.h> /* Define class_create and device_create */

#define DRIVER_AUTHOR "Dai"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"

struct my_device
{
    /* dev_t dev_num = MKDEV(200, 0); static allocate */
    dev_t dev_num;
    struct class *class;

} mdev;

/* Constructor */
static int __init
hello_init(void)
{
    printk(KERN_INFO "Hello Kernel Module\n");

    /* Step 1: Dynamic allocating device number */
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "mdev_num") < 0)
    {
        pr_err("Fail to alloc chrdev regioin\n");
        return -1;
    }
    pr_info("Major = %d, Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    /* Step 2: Create struct class*/
    if ((mdev.class = class_create(THIS_MODULE, "m_class")) == NULL)
    {
        pr_err("Can not create the struct class for my device\n");
        goto rm_device_number;
    }

    /* Create device*/
    if (device_create(mdev.class, NULL, mdev.dev_num, NULL, "m_dev") == NULL)
    {
        pr_err("Can not create device\n");
        goto rm_class;
    }
    return 0;

rm_device_number:
    unregister_chrdev_region(mdev.dev_num, 1);

rm_class:
    class_destroy(mdev.class);
    return -1;
}

/* Destructor */
static void __exit hello_exit(void)
{
    /* Last In First Delete*/

    /* Remove device file*/
    device_destroy(mdev.class, mdev.dev_num);

    /* Remove class */
    class_destroy(mdev.class);

    /* Unallocate device number */
    unregister_chrdev_region(mdev.dev_num, 1);

    printk(KERN_INFO "Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);