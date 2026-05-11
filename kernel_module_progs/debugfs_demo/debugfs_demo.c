#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define DEBUGFS_DIR_NAME "debugfs_demo"
#define DEBUGFS_FILE_NAME "message"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakil");
MODULE_DESCRIPTION("Minimal debugfs demo module");

static struct dentry *demo_dir;
static struct dentry *demo_file;
static char demo_message[128] = "hello from debugfs_demo\n";

static ssize_t demo_read(struct file *file, char __user *user_buf,
			 size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(user_buf, count, ppos, demo_message,
				       strlen(demo_message));
}

static ssize_t demo_write(struct file *file, const char __user *user_buf,
			  size_t count, loff_t *ppos)
{
	size_t len = min(count, sizeof(demo_message) - 1);

	if (copy_from_user(demo_message, user_buf, len))
		return -EFAULT;

	demo_message[len] = '\0';
	return count;
}

static const struct file_operations demo_fops = {
	.owner = THIS_MODULE,
	.read = demo_read,
	.write = demo_write,
};

static int __init debugfs_demo_init(void)
{
	pr_info("debugfs_demo: init\n");

	demo_dir = debugfs_create_dir(DEBUGFS_DIR_NAME, NULL);
	if (IS_ERR_OR_NULL(demo_dir))
		return -ENOMEM;

	demo_file = debugfs_create_file(DEBUGFS_FILE_NAME, 0600, demo_dir,
					NULL, &demo_fops);
	if (IS_ERR_OR_NULL(demo_file)) {
		debugfs_remove_recursive(demo_dir);
		return -ENOMEM;
	}

	pr_info("debugfs_demo: created /sys/kernel/debug/%s/%s\n",
		DEBUGFS_DIR_NAME, DEBUGFS_FILE_NAME);
	return 0;
}

static void __exit debugfs_demo_exit(void)
{
	debugfs_remove_recursive(demo_dir);
	pr_info("debugfs_demo: exit\n");
}

module_init(debugfs_demo_init);
module_exit(debugfs_demo_exit);
