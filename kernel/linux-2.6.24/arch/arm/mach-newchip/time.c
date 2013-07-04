/*
 * DaVinci timer subsystem
 *
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/spinlock.h>

#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/errno.h>
#include <asm/delay.h>
#include <asm/arch/io.h>

#include <asm/arch/debug_uart.h>

static struct clock_event_device clockevent_newchip;

#define APBT_MIN_PERIOD			4
#define APBT_MIN_DELTA_USEC		200

#define APBTMR_N_LOAD_COUNT		0x00
#define APBTMR_N_CURRENT_VALUE		0x04
#define APBTMR_N_CONTROL		0x08
#define APBTMR_N_EOI			0x0c
#define APBTMR_N_INT_STATUS		0x10

#define APBTMRS_INT_STATUS		0xa0
#define APBTMRS_EOI			0xa4
#define APBTMRS_RAW_INT_STATUS		0xa8
#define APBTMRS_COMP_VERSION		0xac

#define APBTMR_CONTROL_ENABLE		(1 << 0)
/* 1: periodic, 0:free running. */
#define APBTMR_CONTROL_MODE_PERIODIC	(1 << 1)
#define APBTMR_CONTROL_INT		(1 << 2)


u32 timer_bases[] = {IO_ADDRESS(TIMER_BASE), IO_ADDRESS(TIMER_BASE)+20};
static int timer_irqs[2] = {
	IRQ7_TIMER0,                                                   
	IRQ8_TIMER0,                                                    
};

#define TID_CLOCKEVENT 	0 
#define TID_CLOCKSOURCE	1 

struct timer_s {
	char *name;
	unsigned int id;
	unsigned long period;
	unsigned long reg_base;
	unsigned long tim_reg;
	unsigned long prd_reg;
	unsigned long enamode_shift;
	struct irqaction irqaction;
};
static struct timer_s timers[];


static unsigned long apbt_readl(int i, unsigned long offs)
{
	return readl(timer_bases[i] + offs);
}

static void apbt_writel(int i, unsigned long val,
		 unsigned long offs)
{
	writel(val, timer_bases[i] + offs);
}

static void apbt_disable_int(int i)
{
	unsigned long ctrl = apbt_readl(i, APBTMR_N_CONTROL);

	ctrl |= APBTMR_CONTROL_INT;
	apbt_writel(i, ctrl, APBTMR_N_CONTROL);
}

static void apbt_eoi(int i)
{
	apbt_readl(i, APBTMR_N_EOI);
}

static void apbt_enable_int(int i)
{
	unsigned long ctrl = apbt_readl(i, APBTMR_N_CONTROL);
	/* clear pending intr */
	apbt_readl(i, APBTMR_N_EOI);
	ctrl &= ~APBTMR_CONTROL_INT;
	apbt_writel(i, ctrl, APBTMR_N_CONTROL);
}

static cycle_t read_cycles(void)
{
	unsigned long current_count;

	current_count = apbt_readl(TID_CLOCKSOURCE, APBTMR_N_CURRENT_VALUE);

	return (cycle_t)~current_count;
}


static void dw_apb_clocksource_start(int i)
{
	/*
	 * start count down from 0xffff_ffff. this is done by toggling the
	 * enable bit then load initial load count to ~0.
	 */
	unsigned long ctrl = apbt_readl(i, APBTMR_N_CONTROL);

	ctrl &= ~APBTMR_CONTROL_ENABLE;
	apbt_writel(i, ctrl, APBTMR_N_CONTROL);
	apbt_writel(i, ~0, APBTMR_N_LOAD_COUNT);
	/* enable, mask interrupt */
	ctrl &= ~APBTMR_CONTROL_MODE_PERIODIC;
	ctrl |= (APBTMR_CONTROL_ENABLE | APBTMR_CONTROL_INT);
	apbt_writel(i, ctrl, APBTMR_N_CONTROL);
	/* read it once to get cached counter value initialized */
	read_cycles();
}


static void apbt_set_mode(enum clock_event_mode mode,
			  struct clock_event_device *evt)
{
	unsigned long ctrl;
	unsigned long period;

	pr_debug("%s CPU %d mode=%d\n", __func__, first_cpu(*evt->cpumask),
		 mode);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		period = DIV_ROUND_UP(CLOCK_TICK_RATE, HZ);
		ctrl = apbt_readl(TID_CLOCKEVENT, APBTMR_N_CONTROL);
		ctrl |= APBTMR_CONTROL_MODE_PERIODIC;
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		/*
		 * DW APB p. 46, have to disable timer before load counter,
		 * may cause sync problem.
		 */
		ctrl &= ~APBTMR_CONTROL_ENABLE;
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		udelay(1);
		pr_debug("Setting clock period %lu for HZ %d\n", period, HZ);
		apbt_writel(TID_CLOCKEVENT, period, APBTMR_N_LOAD_COUNT);
		ctrl |= APBTMR_CONTROL_ENABLE;
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		ctrl = apbt_readl(TID_CLOCKEVENT, APBTMR_N_CONTROL);
		/*
		 * set free running mode, this mode will let timer reload max
		 * timeout which will give time (3min on 25MHz clock) to rearm
		 * the next event, therefore emulate the one-shot mode.
		 */
		ctrl &= ~APBTMR_CONTROL_ENABLE;
		ctrl &= ~APBTMR_CONTROL_MODE_PERIODIC;

		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		/* write again to set free running mode */
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);

		/*
		 * DW APB p. 46, load counter with all 1s before starting free
		 * running mode.
		 */
		apbt_writel(TID_CLOCKEVENT, ~0, APBTMR_N_LOAD_COUNT);
		ctrl &= ~APBTMR_CONTROL_INT;
		ctrl |= APBTMR_CONTROL_ENABLE;
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		break;

	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
		ctrl = apbt_readl(TID_CLOCKEVENT, APBTMR_N_CONTROL);
		ctrl &= ~APBTMR_CONTROL_ENABLE;
		apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
		break;

	case CLOCK_EVT_MODE_RESUME:
		apbt_enable_int(TID_CLOCKEVENT);
		break;
	}
}

static int apbt_next_event(unsigned long delta,
			   struct clock_event_device *evt)
{
	unsigned long ctrl;

	/* Disable timer */
	ctrl = apbt_readl(TID_CLOCKEVENT, APBTMR_N_CONTROL);
	ctrl &= ~APBTMR_CONTROL_ENABLE;
	apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);
	/* write new count */
	apbt_writel(TID_CLOCKEVENT, delta, APBTMR_N_LOAD_COUNT);
	ctrl |= APBTMR_CONTROL_ENABLE;
	apbt_writel(TID_CLOCKEVENT, ctrl, APBTMR_N_CONTROL);

	return 0;
}

static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = &clockevent_newchip;

	apbt_eoi(TID_CLOCKEVENT); //clear the interrupt
	evt->event_handler(evt);
	return IRQ_HANDLED;
}

/* called when 32-bit counter wraps */
static irqreturn_t freerun_interrupt(int irq, void *dev_id)
{
	apbt_eoi(TID_CLOCKSOURCE); //clear the interrupt
	return IRQ_HANDLED;
}

static struct timer_s timers[] = {
	[TID_CLOCKEVENT] = {
		.name      = "clockevent",
		.irqaction = {
			.flags   = IRQF_DISABLED | IRQF_TIMER,
			.handler = timer_interrupt,
		}
	},
	[TID_CLOCKSOURCE] = {
		.name       = "free-run counter",
		.period     = ~0,
		.irqaction = {
			.flags   = IRQF_DISABLED | IRQF_TIMER,
			.handler = freerun_interrupt,
		}
	},
};

static void __init timer_init(void)
{
	int i;

	/* Init of each timer as a 32-bit timer */
	for (i=0; i< ARRAY_SIZE(timers); i++) {
		struct timer_s *t = &timers[i];

		
		apbt_writel(i, 0, APBTMR_N_CONTROL);
		if (t->name) {
			t->id = i;
			/* Register interrupt */
			t->irqaction.name = t->name;
			t->irqaction.dev_id = (void *)t;
			if (t->irqaction.handler != NULL) {
				setup_irq(timer_irqs[t->id], &t->irqaction);
			}
		}
		apbt_enable_int(i);
	}
	dw_apb_clocksource_start(TID_CLOCKSOURCE);
}
static struct clocksource clocksource_newchip = {
	.name		= "timer0_1",
	.rating		= 300,
	.read		= read_cycles,
	.mask		= CLOCKSOURCE_MASK(32),
	.shift		= 24,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static struct clock_event_device clockevent_newchip = {
	.name		= "timer0_0",
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.shift		= 32,
	.set_next_event	= apbt_next_event,
	.set_mode	= apbt_set_mode,
};


static void __init newchip_timer_init(void)
{
	static char err[] __initdata = KERN_ERR
		"%s: can't register clocksource!\n";

	uart_puts("newchip_timer_init\n");
	/* init timer hw */
	timer_init();

	/* setup clocksource */
	clocksource_newchip.mult =
		clocksource_khz2mult(CLOCK_TICK_RATE/1000,
				     clocksource_newchip.shift);
	if (clocksource_register(&clocksource_newchip))
		printk(err, clocksource_newchip.name);

	/* setup clockevent */
	clockevent_newchip.mult = div_sc(CLOCK_TICK_RATE, NSEC_PER_SEC,
					 clockevent_newchip.shift);
	clockevent_newchip.max_delta_ns =
		clockevent_delta2ns(0xfffffffe, &clockevent_newchip);
	clockevent_newchip.min_delta_ns =
		clockevent_delta2ns(1, &clockevent_newchip);

	clockevent_newchip.cpumask = cpumask_of_cpu(0);
	clockevents_register_device(&clockevent_newchip);
}

struct sys_timer newchip_timer = {
	.init   = newchip_timer_init,
};

void davinci_watchdog_reset(void)
{
}

