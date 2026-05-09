#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");  //*
MODULE_AUTHOR("InPyjama - Piyush");  //!
MODULE_DESCRIPTION("Our first dynamically loadable kernel module");  //!

static int pyjama_module_init(void) {
    printk("Hello, World! (from InPyjama!!)\n");
    return 0;
}

static void pyjama_module_exit(void) {
    printk("Good, Bye! (from InPyjama!!)\n");
}

module_init(pyjama_module_init);
module_exit(pyjama_module_exit);
