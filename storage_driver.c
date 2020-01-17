#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>

MODULE_AUTHOR("ANHHT38");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");

static struct class *class_name;
static struct device *device_name;
static dev_t dev;
static struct cdev *my_cdev;
char *kernel_buf;
size_t len_buf_kernel;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file*, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
	printk("open\n");
	return 0;
}
static int dev_close(struct inode *inodep, struct file *filep)
{
	printk("close\n");
	return 0;
}

static ssize_t dev_read(struct file*filep, char __user *buf, size_t len, loff_t *offset)
{
	len = len_buf_kernel;
    if (copy_to_user(buf, kernel_buf, len) == 0)
    {
        printk("read %d byte \n", (int)len);
    }
	return 0;
}
static ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
    len_buf_kernel = len;
	
    

    if((int)len > 1000)
    {
        len = 1000;
    }
    kernel_buf = kmalloc(len, GFP_USER);
    memset(kernel_buf, 0, len);

    if (copy_from_user(kernel_buf, buf, len) == 0)
    {
        printk("write %d byte \n", (int)len);
    }
    *offset = len;
	return len;
}

static int __init exam_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev, 0, 1, "example");
	if (ret)
	{
		printk("can not register major no\n");
		return ret;
	}
	printk(KERN_INFO "register successfully major now is: %d\n", MAJOR(dev));
	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &fops);
	my_cdev->owner = THIS_MODULE;
	my_cdev->ops = &fops;
	my_cdev->dev = dev;
	ret = cdev_add(my_cdev, dev, 1);
	
	if (ret < 0) {
		pr_info("cdev_add error\n");
		return ret;
	}

	class_name = class_create(THIS_MODULE, "tanh_driver");
	if (IS_ERR(class_name)) {
		pr_info("create class failed\n");
		return PTR_ERR(class_name);
	}
	pr_info("create successfully classs\n");

	device_name = device_create(class_name, NULL, dev, NULL, "storage_dev");
	if (IS_ERR(device_name)) {
		pr_info("Create device failed\n");
		return PTR_ERR(device_name);
	}
	pr_info("create device succes\n");
	
	return 0;
}

static void __exit exam_exit(void)
{
	printk("goodbye\n");
	cdev_del(my_cdev);
	device_destroy(class_name, dev);
	class_destroy(class_name);
	unregister_chrdev_region(dev, 1);
}

module_init(exam_init);
module_exit(exam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DRIVER_AUTHOR");
MODULE_DESCRIPTION("DRIVER_DSC");
