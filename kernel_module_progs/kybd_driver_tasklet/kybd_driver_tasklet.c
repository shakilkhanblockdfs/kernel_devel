#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/keyboard.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/version.h>

#define KYBD_RING_SIZE 64

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Educational keyboard notifier driver using a tasklet");

struct kybd_event {
	unsigned int keycode;
	unsigned int shift;
	bool down;
};

static struct tasklet_struct kybd_tasklet;
static DEFINE_SPINLOCK(kybd_lock);

static struct kybd_event kybd_ring[KYBD_RING_SIZE];
static unsigned int kybd_head;
static unsigned int kybd_tail;
static unsigned int kybd_dropped;

static bool kybd_ring_empty(void)
{
	return kybd_head == kybd_tail;
}

static bool kybd_ring_full(void)
{
	return ((kybd_head + 1) % KYBD_RING_SIZE) == kybd_tail;
}

static void kybd_queue_event(unsigned int keycode, bool down, unsigned int shift)
{
	unsigned long flags;

	spin_lock_irqsave(&kybd_lock, flags);

	if (kybd_ring_full()) {
		kybd_dropped++;
		spin_unlock_irqrestore(&kybd_lock, flags);
		return;
	}

	kybd_ring[kybd_head].keycode = keycode;
	kybd_ring[kybd_head].down = down;
	kybd_ring[kybd_head].shift = shift;
	kybd_head = (kybd_head + 1) % KYBD_RING_SIZE;

	spin_unlock_irqrestore(&kybd_lock, flags);
}

static bool kybd_dequeue_event(struct kybd_event *event, unsigned int *dropped)
{
	unsigned long flags;
	bool found;

	spin_lock_irqsave(&kybd_lock, flags);

	if (dropped) {
		*dropped = kybd_dropped;
		kybd_dropped = 0;
	}

	if (kybd_ring_empty()) {
		found = false;
	} else {
		*event = kybd_ring[kybd_tail];
		kybd_tail = (kybd_tail + 1) % KYBD_RING_SIZE;
		found = true;
	}

	spin_unlock_irqrestore(&kybd_lock, flags);
	return found;
}

static unsigned int kybd_take_dropped_count(void)
{
	unsigned long flags;
	unsigned int dropped;

	spin_lock_irqsave(&kybd_lock, flags);
	dropped = kybd_dropped;
	kybd_dropped = 0;
	spin_unlock_irqrestore(&kybd_lock, flags);

	return dropped;
}

static void kybd_tasklet_process(void)
{
	struct kybd_event event;
	unsigned int dropped = kybd_take_dropped_count();

	if (dropped)
		pr_warn("kybd_tasklet: dropped %u keyboard events\n", dropped);

	while (kybd_dequeue_event(&event, NULL)) {
		pr_info("kybd_tasklet: keycode=%u state=%s shift=0x%x\n",
			event.keycode,
			event.down ? "pressed" : "released",
			event.shift);
	}
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
static void kybd_tasklet_handler(struct tasklet_struct *tasklet)
{
	kybd_tasklet_process();
}
#else
static void kybd_tasklet_handler(unsigned long data)
{
	kybd_tasklet_process();
}
#endif

static int kybd_notifier_callback(struct notifier_block *nb,
				  unsigned long code, void *_param)
{
	struct keyboard_notifier_param *param = _param;

	if (!param || code != KBD_KEYCODE)
		return NOTIFY_DONE;

	/*
	 * Keep the notifier path short. Save the event and defer logging to the
	 * tasklet, which runs later in softirq context.
	 */
	kybd_queue_event(param->value, param->down, param->shift);
	tasklet_schedule(&kybd_tasklet);

	return NOTIFY_OK;
}

static struct notifier_block kybd_notifier = {
	.notifier_call = kybd_notifier_callback,
};

static int __init kybd_tasklet_driver_init(void)
{
	int ret;

	pr_info("kybd_tasklet: init\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
	tasklet_setup(&kybd_tasklet, kybd_tasklet_handler);
#else
	tasklet_init(&kybd_tasklet, kybd_tasklet_handler, 0);
#endif

	ret = register_keyboard_notifier(&kybd_notifier);
	if (ret) {
		tasklet_kill(&kybd_tasklet);
		pr_err("kybd_tasklet: failed to register keyboard notifier: %d\n",
		       ret);
		return ret;
	}

	pr_info("kybd_tasklet: registered keyboard notifier\n");
	return 0;
}

static void __exit kybd_tasklet_driver_exit(void)
{
	unregister_keyboard_notifier(&kybd_notifier);
	tasklet_kill(&kybd_tasklet);
	pr_info("kybd_tasklet: exit\n");
}

module_init(kybd_tasklet_driver_init);
module_exit(kybd_tasklet_driver_exit);
