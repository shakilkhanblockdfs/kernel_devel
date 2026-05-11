#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Minimal workqueue demo module");

static struct workqueue_struct *demo_wq;
static struct work_struct demo_work;
static struct timer_list demo_timer;
static unsigned long demo_count;

static void demo_work_handler(struct work_struct *work)
{
	pr_info("workqueue_demo: work handler ran, count=%lu\n", demo_count);

	/*
	 * Workqueue handlers run in process context. Unlike hard IRQ handlers,
	 * tasklets, and softirqs, they may sleep if needed.
	 */
	msleep(20);
}

static void demo_timer_handler(struct timer_list *timer)
{
	demo_count++;
	pr_info("workqueue_demo: queueing work\n");

	queue_work(demo_wq, &demo_work);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(3000));
}

static int __init workqueue_demo_init(void)
{
	pr_info("workqueue_demo: init\n");

	demo_wq = alloc_workqueue("demo_wq", WQ_UNBOUND, 0);
	if (!demo_wq)
		return -ENOMEM;

	INIT_WORK(&demo_work, demo_work_handler);

	timer_setup(&demo_timer, demo_timer_handler, 0);
	mod_timer(&demo_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static void __exit workqueue_demo_exit(void)
{
	del_timer_sync(&demo_timer);
	cancel_work_sync(&demo_work);
	destroy_workqueue(demo_wq);
	pr_info("workqueue_demo: exit\n");
}

module_init(workqueue_demo_init);
module_exit(workqueue_demo_exit);
