/*
 * Sunxi A80 CPUCFG register definition.
 *
 * (C) Copyright 2016 Chen-Yu Tsai <wens@csie.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_CPUCFG_SUN9I_H
#define _SUNXI_CPUCFG_SUN9I_H

#include <linux/compiler.h>
#include <linux/types.h>

#define CPUCFG_CX_CTRL0_L1_RST_DISABLE(core)	BIT(core)

#define CPUCFG_CX_STATUS_STANDBYWFI(core)	BIT(16 + core)

#define CPUCFG_CX_RST_CORE(core)		BIT(core)
#define CPUCFG_CX_RST_NEON(core)		BIT(4 + core) /* A15 only */
#define CPUCFG_CX_RST_L2			BIT(8)
#define CPUCFG_CX_RST_HRESET			BIT(12)
#define CPUCFG_CX_RST_DBG(core)			BIT(16 + core)
#define CPUCFG_CX_RST_ETM(core)			BIT(20 + core)
#define CPUCFG_CX_RST_SOC_DBG			BIT(24)

#ifndef __ASSEMBLY__

struct __packed sunxi_cpucfg_cluster {
	u32 ctrl0;		/* base + 0x0 */
	u32 ctrl1;		/* base + 0x4 */
	u32 adb400_pwrdnreqn;	/* base + 0x8 */
	u8 res[0x4];		/* base + 0xc */
};

struct __packed sunxi_cpucfg_reg {
	struct sunxi_cpucfg_cluster cluster[2];	/* 0x00 */
	u8 res0[0x8];		/* 0x20 */
	u32 gen_ctrl0;		/* 0x28 */
	u32 gen_ctrl1;		/* 0x2c */
	u32 cluster_status[2];	/* 0x30 */
	u8 res1[0x4];		/* 0x38 */
	u32 irq_fiq_status;	/* 0x3c */
	u32 irq_fiq_mask;	/* 0x40 */
	u8 res2[0x3c];		/* 0x44 */
	u32 cluster_reset[2];	/* 0x80 */
	u32 gic_jtag_reset;	/* 0x88 */
};

#endif /* __ASSEMBLY__ */
#endif /* _SUNXI_CPUCFG_SUN9I_H */
