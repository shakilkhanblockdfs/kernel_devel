#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_ENTRY_NAME "pyjama_driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Piyush Itankar <piyush@inpyjama.com>");
MODULE_DESCRIPTION("Basic read loadable kernel module");

static struct proc_dir_entry *custom_proc_node;

static ssize_t pyjama_driver_read(struct file *file, char __user *user_buffer,
				  size_t count, loff_t *offset)
{
	const char message[] = "Hello from pyjama_driver\n";
	size_t message_len = sizeof(message) - 1;

	return simple_read_from_buffer(user_buffer, count, offset, message,
				       message_len);
}

static const struct proc_ops pyjama_driver_proc_ops = {
	.proc_read = pyjama_driver_read,
};

static int __init pyjama_module_init(void)
{
	pr_info("pyjama_module_init: entry\n");

	custom_proc_node = proc_create(PROC_ENTRY_NAME, 0444, NULL,
				       &pyjama_driver_proc_ops);
	if (!custom_proc_node) {
		pr_err("pyjama_module_init: failed to create /proc/%s\n",
		       PROC_ENTRY_NAME);
		return -ENOMEM;
	}

	pr_info("pyjama_module_init: created /proc/%s\n", PROC_ENTRY_NAME);
	pr_info("pyjama_module_init: exit\n");

	return 0;
}

static void __exit pyjama_module_exit(void)
{
	pr_info("pyjama_module_exit: entry\n");

	proc_remove(custom_proc_node);

	pr_info("pyjama_module_exit: removed /proc/%s\n", PROC_ENTRY_NAME);
	pr_info("pyjama_module_exit: exit\n");
}

module_init(pyjama_module_init);
module_exit(pyjama_module_exit);
