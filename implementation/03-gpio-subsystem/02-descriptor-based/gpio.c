#include <linux/module.h>           /* Defines funtions such as module_init/module_exit */
#include <linux/gpio.h>             /* Defines functions such as gpio_request/gpio_free */
#include <linux/platform_device.h>  /* For platform devices */
#include <linux/gpio/consumer.h>    /* For GPIO Descriptor */
#include <linux/of.h>               /* For DT */  


#define DRIVER_AUTHOR   "Dai dinhdai0904@gmail.com"
#define DRIVER_DESC     "GPIO Subsystem - Descriptor Based"

#define GPIO1_16    48      /* 32*1 + 16 ~ P9_15*/
#define LOW         0   
#define HIGH        1

struct gpio_desc *gpio1_16;

static const struct of_device_id gpio_dts_ids[] = {
    { .compatible = "gpio-descriptor-based", },
    {}
};

static int my_platfrom_drv_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    gpio1_16 = gpiod_get(dev, "myled48", GPIOD_OUT_LOW);
    gpiod_set_value(gpio1_16, HIGH);

    pr_info("[DAI] FUNC: %s ----- LINE: %d\n", __func__, __LINE__);
    return 0;
}

static int my_platform_drv_remove(struct platform_device *pdev)
{
    gpiod_set_value(gpio1_16, LOW);
    gpiod_put(gpio1_16);

    pr_info("[DAI] FUNC: %s ----- LINE: %d\n", __func__,__LINE__);
    
    return 0;
}


/* platform driver */
static struct platform_driver my_platform_drv = {
    .probe = my_platfrom_drv_probe,
    .remove = my_platform_drv_remove,
    .driver = {
        .name = "descriptor-based",
        .of_match_table = of_match_ptr(gpio_dts_ids),
        .owner = THIS_MODULE,
    },
};

module_platform_driver(my_platform_drv);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION("1.0"); 