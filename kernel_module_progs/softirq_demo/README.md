# softirq_demo

This folder contains two softirq-related examples.

## `softirq_demo.c`

An educational in-tree style example showing the conceptual APIs:

- `open_softirq()`
- `raise_softirq()`
- `raise_softirq_irqoff()`

Custom softirq vectors are not a normal out-of-tree module API. Real custom
softirq registration requires kernel-core changes, including adding a vector to
the kernel softirq enum.

## `tasklet_backed_softirq_module.c`

A loadable demonstration that uses a tasklet. Tasklets run from
`TASKLET_SOFTIRQ`, so this is the practical out-of-tree way to observe
softirq-backed deferred execution without modifying core kernel code.

Key rules:

- Softirq context cannot sleep.
- Softirq handlers must be short.
- Use spinlocks for shared data.
- Use workqueues or threaded IRQs when deferred work needs to sleep.
