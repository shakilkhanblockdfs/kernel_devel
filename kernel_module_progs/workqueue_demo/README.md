# workqueue_demo

Minimal workqueue demo.

The module starts a timer. Each timer expiry queues work onto a private
workqueue. The work handler logs a message and calls `msleep(20)` to show that
workqueue handlers run in sleepable process context.

Key APIs:

- `alloc_workqueue()`
- `INIT_WORK()`
- `queue_work()`
- `cancel_work_sync()`
- `destroy_workqueue()`

Build/load on a Linux system with kernel headers:

```sh
make
sudo insmod workqueue_demo.ko
sudo dmesg -w
sudo rmmod workqueue_demo
```
