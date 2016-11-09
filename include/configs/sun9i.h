/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * Configuration settings for the Allwinner A80 (sun9i) CPU
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * A80 specific configuration
 */

/* First 4 kiB is reserved for CPU0 hotplug flags */
#define CONFIG_ARMV7_SECURE_BASE	(SUNXI_SRAM_B_BASE + 4 * 1024)
#define CONFIG_ARMV7_SECURE_MAX_SIZE	(252 * 1024) /* 252 KB */

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#endif /* __CONFIG_H */
