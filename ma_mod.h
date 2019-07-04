/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Unlimited memory access driver.
 *
 * Copyright (c) 2019 Katsuhiro Suzuki.
 */

#ifndef MA_MOD_H__
#define MA_MOD_H__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define MA_DRV_NAME    "ma"
#define MA_DEV_NAME    "ma"

struct ma_dev {
	struct mutex mut;

	dev_t id;
	struct cdev cdev;
	struct class *class;
	struct device *dev;
};

#endif //MA_MOD_H__
