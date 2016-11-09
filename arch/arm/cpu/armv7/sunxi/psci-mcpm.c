/*
 * Copyright (C) 2016
 * Author: Chen-Yu Tsai <wens@csie.org>
 *
 * Based on assembly code by Marc Zyngier <marc.zyngier@arm.com>,
 * which was based on code by Carl van Schaik <carl@ok-labs.com>.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <config.h>
#include <common.h>

#include <asm/arch/cpu.h>
#include <asm/arch/cpucfg_sun9i.h>
#include <asm/arch/prcm_sun9i.h>
#include <asm/armv7.h>
#include <asm/io.h>
#include <asm/psci.h>
#include <asm/secure.h>

#include <linux/bitops.h>

/*
 * NOTE dense CPU IDs (0~3 for first cluster of 4 cores, 4~7 for the
 * second cluster) are used throughout the PSCI code. Any MPIDR style
 * values must be converted.
 */

/*
 * Provide a dense CPU ID for 2-cluster systems. This must be coded in
 * assembly as it gets called from psci_stack_setup, when the stack isn't
 * available yet.
 *
 * Only r0 and r3 is usable. r8 - r12 are available if this function is
 * only called from psci_stack_setup, which we cannot guarantee.
 */
u32 __secure __naked psci_get_cpu_id(void)
{
	asm volatile (
		"mrc	p15, 0, r3, c0, c0, 5	@ Get MPIDR\n"
		"lsr	r0, r3, #6\n"
		"and	r3, r3, #3\n"
		"and	r0, r0, #4\n"
		"orr	r0, r0, r3\n"
		"bx	lr\n"
	);

	/*
	 * The last five lines are the compiler generated assembly code for
	 *
	 *	return (reg & 0x3) | (((reg >> 8) & 0x1) << 2);
	 *
	 * We can't guarantee that all compilers correctly use only r0 and
	 * r3, so we use inline assembly here.
	 */
}

static void __secure cp15_write_cntp_tval(u32 tval)
{
	asm volatile ("mcr p15, 0, %0, c14, c2, 0" : : "r" (tval));
}

static void __secure cp15_write_cntp_ctl(u32 val)
{
	asm volatile ("mcr p15, 0, %0, c14, c2, 1" : : "r" (val));
}

static u32 __secure cp15_read_cntp_ctl(void)
{
	u32 val;

	asm volatile ("mrc p15, 0, %0, c14, c2, 1" : "=r" (val));

	return val;
}

#define ONE_US (CONFIG_TIMER_CLK_FREQ / 1000000)

/* Use a different name to avoid clashing with the non-secure function */
static void __secure __udelay_sec(unsigned long us)
{
	u32 reg = ONE_US * us;

	cp15_write_cntp_tval(reg);
	isb();
	cp15_write_cntp_ctl(3);

	do {
		isb();
		reg = cp15_read_cntp_ctl();
	} while (!(reg & BIT(2)));

	cp15_write_cntp_ctl(0);
	isb();
}

static void __secure clamp_release(u32 *clamp)
{
	writel(0xff, clamp);
	__udelay_sec(10);
	writel(0xfe, clamp);
	__udelay_sec(10);
	writel(0xf8, clamp);
	__udelay_sec(10);
	writel(0xf0, clamp);
	__udelay_sec(10);
	writel(0x00, clamp);
}

static void __secure clamp_set(u32 *clamp)
{
	writel(0xff, clamp);
}

static void __secure sunxi_core_power_switch(u32 *clamp, u32 *pwroff,
					     bool on, int cpu)
{
	if (on) {
		/* Release power clamp */
		clamp_release(clamp);

		__udelay_sec(20);

		/* Clear power gating */
		clrbits_le32(pwroff, BIT(cpu));
	} else {
		/* Set power gating */
		setbits_le32(pwroff, BIT(cpu));

		__udelay_sec(20);

		/* Activate power clamp */
		clamp_set(clamp);
	}
}

static void __secure sunxi_cpu_set_power(int cpu, bool on)
{
	struct sunxi_prcm_reg *prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;
	int cluster = (cpu >> 2) & 0x1;
	int core = cpu & 0x3;

	sunxi_core_power_switch(&prcm->cpu_pwr_clamp[cluster][core],
				&prcm->cpu_pwroff[cluster], on, core);
}

static u32 __secure cp15_read_scr(void)
{
	u32 scr;

	asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r" (scr));

	return scr;
}

static void __secure cp15_write_scr(u32 scr)
{
	asm volatile ("mcr p15, 0, %0, c1, c1, 0" : : "r" (scr));
	isb();
}

int __secure psci_cpu_on(u32 __always_unused unused, u32 mpidr, u32 pc)
{
	struct sunxi_cpucfg_reg *cpucfg =
		(struct sunxi_cpucfg_reg *)SUNXI_CPUCFG_BASE;
	struct sunxi_prcm_reg *prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;
	u32 cluster = (mpidr >> 8) & 0x1;
	u32 cpu = mpidr & 0x3;
	u32 cpuid = cpu | (cluster << 2);

	/* TODO We don't support multi-cluster yet */
	if (cluster > 0)
		return ARM_PSCI_RET_INVAL;

	/* store target PC */
	psci_save_target_pc(cpuid, pc);

	/* Set secondary core power on PC */
	writel((u32)&psci_cpu_entry, &prcm->cpu_soft_entry);

	/* Assert power-on reset on target CPU */
	clrbits_le32(&prcm->cpu_rst[cluster], BIT(cpu));

	/* Cortex-A7: hold L1 cache reset disable signal low */
	if (cluster == 0)
		clrbits_le32(&cpucfg->cluster[cluster].ctrl0,
			     CPUCFG_CX_CTRL0_L1_RST_DISABLE(cpu));

	/* Lock CPU (Disable external debug access) */
	clrbits_le32(&cpucfg->cluster_reset[cluster],
		     CPUCFG_CX_RST_DBG(cpu));

	/* Cortex-A7: Assert ETM reset */
	if (cluster == 0)
		clrbits_le32(&cpucfg->cluster_reset[cluster],
			     CPUCFG_CX_RST_ETM(cpu));

	/*
	 * Allwinner code also asserts resets for NEON on A15. According
	 * to ARM manuals, asserting power-on reset is sufficient.
	 */

	/* Power up target CPU */
	sunxi_cpu_set_power(cpu, true);

	/* De-assert power-on reset on target CPU */
	setbits_le32(&prcm->cpu_rst[cluster], BIT(cpu));

	/* De-assert core reset on target CPU */
	setbits_le32(&cpucfg->cluster_reset[cluster],
		     CPUCFG_CX_RST_CORE(cpu));

	/* Cortex-A7: De-assert ETM reset */
	if (cluster == 0)
		setbits_le32(&cpucfg->cluster_reset[cluster],
			     CPUCFG_CX_RST_ETM(cpu));

	/* Unlock CPU (Disable external debug access) */
	setbits_le32(&cpucfg->cluster_reset[cluster],
		     CPUCFG_CX_RST_DBG(cpu));

	return ARM_PSCI_RET_SUCCESS;
}

void __secure psci_arch_init(void)
{
	u32 reg;

	reg = cp15_read_scr();
	reg &= ~BIT(0); /* Secure mode */
	cp15_write_scr(reg);
}
