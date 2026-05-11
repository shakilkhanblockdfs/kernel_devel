#include <linux/init.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Educational keyboard notifier kernel module");

/*
 * This module observes keyboard events through the Linux keyboard notifier
 * chain. It does not replace the real keyboard/input driver and it does not
 * consume events, so normal keyboard input continues to work.
 */

static const char *kybd_event_name(unsigned long code)
{
	switch (code) {
	case KBD_KEYCODE:
		return "keycode";
	case KBD_UNBOUND_KEYCODE:
		return "unbound-keycode";
	case KBD_UNICODE:
		return "unicode";
	case KBD_KEYSYM:
		return "keysym";
	case KBD_POST_KEYSYM:
		return "post-keysym";
	default:
		return "unknown";
	}
}

static int kybd_notifier_callback(struct notifier_block *nb,
				  unsigned long code, void *_param)
{
	struct keyboard_notifier_param *param = _param;

	if (!param)
		return NOTIFY_DONE;

	/*
	 * KBD_KEYCODE is the most useful stage for a simple demo because it
	 * reports the Linux input keycode and whether this is press/release.
	 */
	if (code == KBD_KEYCODE) {
		pr_info("kybd_driver: keycode=%u state=%s shift=0x%x down=%d\n",
			param->value,
			param->down ? "pressed" : "released",
			param->shift,
			param->down);
		return NOTIFY_OK;
	}

	pr_debug("kybd_driver: event=%s value=%u down=%d shift=0x%x\n",
		 kybd_event_name(code), param->value, param->down,
		 param->shift);

	return NOTIFY_DONE;
}

static struct notifier_block kybd_notifier = {
	.notifier_call = kybd_notifier_callback,
};

static int __init kybd_driver_init(void)
{
	int ret;

	pr_info("kybd_driver: init\n");

	ret = register_keyboard_notifier(&kybd_notifier);
	if (ret) {
		pr_err("kybd_driver: failed to register keyboard notifier: %d\n",
		       ret);
		return ret;
	}

	pr_info("kybd_driver: registered keyboard notifier\n");
	return 0;
}

static void __exit kybd_driver_exit(void)
{
	unregister_keyboard_notifier(&kybd_notifier);
	pr_info("kybd_driver: unregistered keyboard notifier\n");
}

module_init(kybd_driver_init);
module_exit(kybd_driver_exit);
