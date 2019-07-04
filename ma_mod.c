// SPDX-License-Identifier: GPL-2.0
/*
 * Unlimited memory access driver.
 *
 * Copyright (c) 2019 Katsuhiro Suzuki.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

#include "ma_mod.h"

static struct ma_dev g_dev;

int ma_fops_mmap(struct file *filp, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	int result;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	result = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
		size, vma->vm_page_prot);
	if (result != 0)
		goto err_out;

	return 0;

err_out:
	return result;
}

int ma_fops_open(struct inode *inode, struct file *filp)
{
	struct ma_dev *d = &g_dev;
	int result;

	result = mutex_lock_interruptible(&d->mut);
	if (result != 0)
		return -ERESTARTSYS;
	filp->private_data = d;
	mutex_unlock(&d->mut);

	return 0;
}

static const struct file_operations ma_fops = {
	.owner = THIS_MODULE,
	.mmap  = ma_fops_mmap,
	.open  = ma_fops_open,
};

static int __init ma_driver_init(void)
{
	struct ma_dev *d = &g_dev;
	int result;

	mutex_init(&d->mut);

	result = alloc_chrdev_region(&d->id, 0, 1, MA_DRV_NAME);
	if (result)
		goto err_out;

	cdev_init(&d->cdev, &ma_fops);
	result = cdev_add(&d->cdev, d->id, 1);
	if (result)
		goto err_region;

	d->class = class_create(THIS_MODULE, MA_DEV_NAME);
	if (IS_ERR(d->class)) {
		result = PTR_ERR(d->class);
		goto err_cdev;
	}

	d->dev = device_create(d->class, NULL, d->id, NULL, MA_DEV_NAME);
	if (IS_ERR(d->dev)) {
		result = PTR_ERR(d->dev);
		goto err_class;
	}

	dev_info(d->dev, "loaded.\n");

	return 0;

err_class:
	class_destroy(d->class);
err_cdev:
	cdev_del(&d->cdev);
err_region:
	unregister_chrdev_region(d->id, 1);
err_out:
	return result;
}

static void __exit ma_driver_exit(void)
{
	struct ma_dev *d = &g_dev;

	dev_info(d->dev, "unload.\n");

	device_destroy(d->class, d->id);
	class_destroy(d->class);
	cdev_del(&d->cdev);
	unregister_chrdev_region(d->id, 1);
}

module_init(ma_driver_init);
module_exit(ma_driver_exit);

MODULE_AUTHOR("Katsuhiro Suzuki <katsuhiro@katsuster.net>");
MODULE_DESCRIPTION("Memory Access Driver");
MODULE_LICENSE("GPL");
