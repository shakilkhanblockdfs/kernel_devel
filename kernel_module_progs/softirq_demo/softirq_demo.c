/*
 * Educational softirq demo.
 *
 * IMPORTANT:
 * Custom softirq handlers are not a normal out-of-tree module API.
 * open_softirq() is for core/in-tree kernel code, and the softirq vector list
 * is fixed by the kernel. This file is intentionally written as an in-kernel
 * style example to show the mechanics, not as a loadable module you should
 * expect to build externally.
 */

#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>

/*
 * A real kernel change would require adding a new vector to the softirq
 * enum in include/linux/interrupt.h. For teaching, imagine this exists:
 *
 *     DEMO_SOFTIRQ
 */

static void demo_softirq_handler(struct softirq_action *action)
{
	pr_info("softirq_demo: custom softirq handler ran\n");

	/*
	 * Softirq context cannot sleep. Keep work short and use spinlocks for
	 * shared data if needed.
	 */
}

static int __init softirq_demo_core_init(void)
{
	pr_info("softirq_demo: registering custom softirq handler\n");

	/*
	 * In real in-tree kernel code:
	 *
	 *     open_softirq(DEMO_SOFTIRQ, demo_softirq_handler);
	 *     raise_softirq(DEMO_SOFTIRQ);
	 */

	return 0;
}

/*
 * Example call site from interrupt or kernel context:
 *
 *     raise_softirq(DEMO_SOFTIRQ);
 *
 * If interrupts are already disabled:
 *
 *     raise_softirq_irqoff(DEMO_SOFTIRQ);
 */
