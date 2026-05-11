#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Minimal tasklet demo module");

static struct tasklet_struct demo_tasklet;
static struct timer_list demo_timer;
static unsigned long demo_count;

static void demo_tasklet_body(void)
{
	pr_info("tasklet_demo: tasklet ran, count=%lu\n", demo_count);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
static void demo_tasklet_handler(struct tasklet_struct *tasklet)
{
	demo_tasklet_body();
}
#else
static void demo_tasklet_handler(unsigned long data)
{
	demo_tasklet_body();
}
#endif

static void demo_timer_handler(struct timer_list *timer)
{
	demo_count++;
	pr_info("tasklet_demo: scheduling tasklet\n");

	tasklet_schedule(&demo_tasklet);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(3000));
}

static int __init tasklet_demo_init(void)
{
	pr_info("tasklet_demo: init\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
	tasklet_setup(&demo_tasklet, demo_tasklet_handler);
#else
	tasklet_init(&demo_tasklet, demo_tasklet_handler, 0);
#endif

	timer_setup(&demo_timer, demo_timer_handler, 0);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static void __exit tasklet_demo_exit(void)
{
	del_timer_sync(&demo_timer);
	tasklet_kill(&demo_tasklet);
	pr_info("tasklet_demo: exit\n");
}

module_init(tasklet_demo_init);
module_exit(tasklet_demo_exit);
