/*
 * Sunxi A80 Power Reset and Clock Module register definition
 *
 * (C) Copyright 2016 Chen-Yu Tsai <wens@csie.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_PRCM_SUN9I_H
#define _SUNXI_PRCM_SUN9I_H

struct __packed sunxi_prcm_reg {
	u32 cpus_rst;		/* 0x000 */
	u32 cpu_rst[2];		/* 0x004 */
	u8 res0[0x4];		/* 0x00c */
	u32 cpus_cfg;		/* 0x010 */
	u8 res1[0x8];		/* 0x014 */
	u32 apbs_ratio;		/* 0x01c */
	u8 res2[0x8];		/* 0x020 */
	u32 apbs_gate;		/* 0x028 */
	u8 res3[0x18];		/* 0x02c */
	u32 pll_ctrl1;		/* 0x044 */
	u8 res4[0xc];		/* 0x048 */
	u32 clk_cir;		/* 0x054 */
	u32 clk_i2s0;		/* 0x058 */
	u32 clk_i2s1;		/* 0x05c */
	u8 res5[0x50];		/* 0x060 */
	u32 apb0_reset;		/* 0x0b0 */
	u8 res6[0x4c];		/* 0x0b4 */
	u32 cpu_pwroff[2];	/* 0x100 */
	u8 res7[0x8];		/* 0x108 */
	u32 vdd_sys_pwroff;	/* 0x110 */
	u8 res8[0x4];		/* 0x114 */
	u32 gpu_pwroff;		/* 0x118 */
	u8 res9[0x4];		/* 0x11c */
	u32 vdd_sys_rst;	/* 0x120 */
	u8 res10[0x1c];		/* 0x124 */
	u32 cpu_pwr_clamp[2][4]; /* 0x140 */
	u32 super_standby_flag;	/* 0x160 */
	u32 cpu_soft_entry;	/* 0x164 */
	u32 super_standby_entry; /* 0x168 */
	u8 res11[0x34];		/* 0x16c */
	u32 nmi_irq_ctrl;	/* 0x1a0 */
	u32 nmi_irq_en;		/* 0x1a4 */
	u32 nmi_irq_status;	/* 0x1a8 */
	u8 res12[0x14];		/* 0x1ac */
	u32 pll_audio_ctrl;	/* 0x1c0 */
	u32 pll_audio_bias;	/* 0x1c4 */
	u32 pll_audio_pat_cfg;	/* 0x1c8 */
	u32 pll_audio_ctrl_sw;	/* 0x1cc */
	u8 res13[0x20];		/* 0x1d0 */
	u32 osc24m_ctrl;	/* 0x1f0 */
};

#endif /* _SUNXI_PRCM_SUN9I_H */
