/*
    @brief: File operations with character driver
    @author: Nguyen Dinh Dai
 */

/**********************************************************************************
 *                                      INCLUDES
 **********************************************************************************/

#include <linux/module.h>  /* Define module_init/module_exit */
#include <linux/fs.h>      /* Allocate major and minor number */
#include <linux/device.h>  /* Define class_create/device_create */
#include <linux/cdev.h>    /* Define cdev_init/cdev_add */
#include <linux/slab.h>    /* Define kmalloc */
#include <linux/uaccess.h> /* Define copy_to_user/copy_from_user */

/**********************************************************************************
 *                                      DEFINES
 **********************************************************************************/

#define DRIVER_AUTHOR "Dai"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"

#define NPAGES  1
#define F_SIZE  NPAGES * PAGE_SIZE

/**********************************************************************************
 *                                      STRUCTURE
 **********************************************************************************/

struct my_device
{
    /* dev_t dev_num = MKDEV(200, 0); static allocate */
    dev_t dev_num;
    struct class *class;
    struct cdev m_cdev;
    char *kmalloc_ptr;
    size_t size;

} mdev;

/**********************************************************************************
 *                                      FUNCTIONS PROTOTYPE
 **********************************************************************************/

static int m_open(struct inode *inode, struct file *filp);
static int m_release(struct inode *inode, struct file *file);
static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
static ssize_t m_write(struct file *filp, const char *user_buf, size_t size, loff_t *offset);
static loff_t m_llseek(struct file *filp, loff_t offset, int whence);

/**********************************************************************************
 *                                      FUNCTIONS
 **********************************************************************************/

/* If user space call read or write,... => call m_read, m_write,... in kernel */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = m_read,
    .write = m_write,
    .open = m_open,
    .release = m_release,
    .llseek = m_llseek,
};

/* This function will be called when we open the Device file */
static int m_open(struct inode *inode, struct file *filp)
{
    pr_info("[DAI] System call open() called...!!!\n");
    return 0;
}

/* This function will be called when we close the Device file */
static int m_release(struct inode *inode, struct file *file)
{
    pr_info("[DAI] System call close() called...!!!\n");
    return 0;
}

/* This function will be called when we read the Device file */
static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_read;
    pr_info("[DAI] System call read() called...!!!\n");
    pr_info("[DAI] Read requested for %ld bytes.\n", size);

    /* Check size */
    to_read = (size > mdev.size - *offset) ? (mdev.size - *offset) : size;

    /* Copy to user*/
    if (copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
        return -EFAULT;

    /* Update offset */
    *offset += to_read;

    return to_read;
}

/* This function will be called when we write the Device file */
static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{
    size_t to_write;
    pr_info("[DAI] System call write() called...!!!\n");

    /* Check if number of bytes to write is bigger than MAX bytes */
    to_write = (size + *offset > F_SIZE) ? (F_SIZE - *offset) : size;

    if(!size)
    {
        pr_err("[DAI] No space left on this device.\n");
        return -ENOMEM;
    }

    /* Copy from user buffer to mapped aread */
    memset(mdev.kmalloc_ptr, 0, F_SIZE);
    /* use copy_from_user() to copy from user buffer to kernel
       this function is similar to strcpy in user space
    */
    if (copy_from_user(mdev.kmalloc_ptr + *offset, user_buf, to_write) != 0)
        return -EFAULT;

    pr_info("[DAI] Data from user: %s\n", mdev.kmalloc_ptr);

    /* Update offset */
    *offset += to_write;
    mdev.size = *offset;

    return size;
}

static loff_t m_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;
    pr_info("[DAI] System call lseek() called...!!!\n");
    pr_info("[DAI] Current file position: %lld.\n:", filp->f_pos);
    switch(whence)
    {
        case SEEK_SET:
            if(offset > F_SIZE || offset < 0)
                return -EINVAL;
            filp->f_pos = offset;
            break;

        case SEEK_CUR:
            temp = filp->f_pos + offset;
            if(temp > F_SIZE || temp < 0)
                return -EINVAL;
            filp->f_pos = temp;
            break;

        case SEEK_END:
            temp = F_SIZE + offset;
            if(temp > F_SIZE || temp < 0)
                return -EINVAL;
            filp->f_pos = temp;
            break;

        default:
            return -EINVAL;
    }

    return filp->f_pos;
}

/* Constructor */
static int __init
hello_init(void)
{
    printk(KERN_INFO "[DAI] Hello Kernel Module\n");

    /* Step 1: Dynamic allocating device number */
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "m_devnum") < 0)
    {
        pr_err("[DAI] Failed to alloc chrdev region\n");
        return -1;
    }
    pr_info("[DAI] Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    /* Step 2: Create struct class*/
    if ((mdev.class = class_create(THIS_MODULE, "m_class")) == NULL)
    {
        pr_err("[DAI] Cannot create the struct class for my device\n");
        goto rm_device_numb;
    }

    /* Step 3: Create device*/
    if ((device_create(mdev.class, NULL, mdev.dev_num, NULL, "m_device")) == NULL)
    {
        pr_err("[DAI] Cannot create my device\n");
        goto rm_class;
    }

    /* Step 4.1: Create character device file structure (cdev) */
    cdev_init(&mdev.m_cdev, &fops);

    /* Step 4.2: Add character device to system */
    if ((cdev_add(&mdev.m_cdev, mdev.dev_num, 1)) < 0)
    {
        pr_err("[DAI] Cannot add the device to the system\n");
        goto rm_device;
    }

    /* Step 5: Create kernel buffer */
    if (mdev.kmalloc_ptr = kmalloc(F_SIZE, GFP_KERNEL), mdev.kmalloc_ptr == 0)
    {
        pr_err("[DAI] Can not allocate memory in kernel\n");
        goto rm_cdev;
    }
    return 0;

rm_cdev:
    cdev_del(&mdev.m_cdev);

rm_device:
    device_destroy(mdev.class, mdev.dev_num);

rm_class:
    class_destroy(mdev.class);

rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);

    return -1;
}

/* Destructor */
static void __exit hello_exit(void)
{
    /* Last In First Delete*/
    /* Deallocate kernel buffer */
    kfree(mdev.kmalloc_ptr);

    /* Remove character device */
    cdev_del(&mdev.m_cdev);

    /* Remove device file*/
    device_destroy(mdev.class, mdev.dev_num);

    /* Remove class */
    class_destroy(mdev.class);

    /* Unallocate device number */
    unregister_chrdev_region(mdev.dev_num, 1);

    printk(KERN_INFO "[DAI] Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);
