# Keyboard Driver Using a Tasklet

This is an educational Linux kernel module that observes keyboard events with a
keyboard notifier and defers the actual event logging to a tasklet.

The module does not replace the real keyboard driver and does not consume key
events. Normal keyboard input continues to work.

## Files

- `kybd_driver_tasklet.c` - tasklet-based keyboard notifier module
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
sudo insmod kybd_driver_tasklet.ko
sudo dmesg -w
```

Press and release keys. The module logs key events from the tasklet:

```text
kybd_tasklet: keycode=30 state=pressed shift=0x0
kybd_tasklet: keycode=30 state=released shift=0x0
```

## Unload

```sh
sudo rmmod kybd_driver_tasklet
```

## Design

The keyboard notifier callback is kept short:

```text
keyboard notifier callback
  -> copy keycode/down/shift into a small ring buffer
  -> tasklet_schedule()
  -> return
```

The tasklet runs later in softirq context:

```text
tasklet handler
  -> drain queued keyboard events
  -> print events with pr_info()
```

The ring buffer is protected with `spin_lock_irqsave()` because the notifier and
tasklet can run on different CPUs or with different interrupt state.

## Important Notes

- Tasklets run in softirq context.
- Tasklets cannot sleep.
- Do not take mutexes in the tasklet handler.
- Keep tasklet work short.
- Use `tasklet_kill()` before unloading so the tasklet cannot run after module
  memory is gone.
- For new production drivers that need sleepable deferred work, threaded IRQs
  or workqueues are usually preferred over tasklets.
