# Keyboard Notifier Driver

This is an educational out-of-tree Linux kernel module that observes keyboard
events through the Linux keyboard notifier chain.

It does not replace the real keyboard driver and it does not consume key
events. Normal keyboard input continues to work.

## Files

- `kybd_driver.c` - kernel module source
- `Makefile` - out-of-tree kernel module build file

## Build

```sh
make
```

This expects kernel headers/build files for the running kernel at:

```text
/lib/modules/$(uname -r)/build
```

## Load

```sh
sudo insmod kybd_driver.ko
```

Watch logs:

```sh
sudo dmesg -w
```

Press and release keys. You should see messages like:

```text
kybd_driver: keycode=30 state=pressed shift=0x0 down=1
kybd_driver: keycode=30 state=released shift=0x0 down=0
```

## Unload

```sh
sudo rmmod kybd_driver
```

## How It Works

The module registers a `notifier_block` using:

```c
register_keyboard_notifier(&kybd_notifier);
```

The callback receives events such as:

- `KBD_KEYCODE`
- `KBD_UNBOUND_KEYCODE`
- `KBD_UNICODE`
- `KBD_KEYSYM`
- `KBD_POST_KEYSYM`

For this example, the module logs only `KBD_KEYCODE` events with `pr_info()`.
Other stages are logged with `pr_debug()`.

## Important Notes

- The callback runs in kernel context and must be fast.
- Do not sleep inside the notifier callback.
- Do not do heavy work directly in the callback.
- For heavier processing, capture minimal state and defer work to a workqueue.
- Keyboard notifier support is useful for learning, debugging, and special
  kernel-side handling, but normal keyboard drivers should use the Linux input
  subsystem.
