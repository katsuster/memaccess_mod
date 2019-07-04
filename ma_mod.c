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
#include <linux/mm.h>

#include "ma_mod.h"

static struct ma_dev g_dev;

int ma_fops_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return 0;
}

int ma_fops_open(struct inode *inode, struct file *filp)
{
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

	return 0;

err_region:
	unregister_chrdev_region(d->id, 1);
err_out:
	return result;
}

static void __exit ma_driver_exit(void)
{
	struct ma_dev *d = &g_dev;

	cdev_del(&d->cdev);
	unregister_chrdev_region(d->id, 1);
}

module_init(ma_driver_init);
module_exit(ma_driver_exit);

MODULE_AUTHOR("Katsuhiro Suzuki <katsuhiro@katsuster.net>");
MODULE_DESCRIPTION("Memory Access Driver");
MODULE_LICENSE("GPL");
