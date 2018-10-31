/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <misc/printk.h>

static int metadata __attribute__ ((section (".metadata"))) __attribute__ ((__used__)) = 6;

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
}
