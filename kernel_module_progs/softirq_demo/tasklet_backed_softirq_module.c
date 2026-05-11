#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Loadable module showing softirq-backed tasklet execution");

static struct tasklet_struct demo_tasklet;
static struct timer_list demo_timer;

static void demo_softirq_backed_work(void)
{
	pr_info("softirq_demo: tasklet ran from TASKLET_SOFTIRQ context\n");
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
static void demo_tasklet_handler(struct tasklet_struct *tasklet)
{
	demo_softirq_backed_work();
}
#else
static void demo_tasklet_handler(unsigned long data)
{
	demo_softirq_backed_work();
}
#endif

static void demo_timer_handler(struct timer_list *timer)
{
	tasklet_schedule(&demo_tasklet);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(3000));
}

static int __init softirq_tasklet_demo_init(void)
{
	pr_info("softirq_demo: init\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
	tasklet_setup(&demo_tasklet, demo_tasklet_handler);
#else
	tasklet_init(&demo_tasklet, demo_tasklet_handler, 0);
#endif

	timer_setup(&demo_timer, demo_timer_handler, 0);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static void __exit softirq_tasklet_demo_exit(void)
{
	del_timer_sync(&demo_timer);
	tasklet_kill(&demo_tasklet);
	pr_info("softirq_demo: exit\n");
}

module_init(softirq_tasklet_demo_init);
module_exit(softirq_tasklet_demo_exit);
