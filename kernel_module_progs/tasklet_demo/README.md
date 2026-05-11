# tasklet_demo

Minimal tasklet demo.

The module starts a timer. Each timer expiry schedules a tasklet with
`tasklet_schedule()`. The tasklet logs a message from softirq context.

Key APIs:

- `tasklet_setup()` or legacy `tasklet_init()`
- `tasklet_schedule()`
- `tasklet_kill()`

Build/load on a Linux system with kernel headers:

```sh
make
sudo insmod tasklet_demo.ko
sudo dmesg -w
sudo rmmod tasklet_demo
```
