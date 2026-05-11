# debugfs_demo

Minimal debugfs demo.

Creates:

```text
/sys/kernel/debug/debugfs_demo/message
```

Example usage after loading:

```sh
sudo mount -t debugfs none /sys/kernel/debug
sudo cat /sys/kernel/debug/debugfs_demo/message
echo "new message" | sudo tee /sys/kernel/debug/debugfs_demo/message
```

Key APIs:

- `debugfs_create_dir()`
- `debugfs_create_file()`
- `simple_read_from_buffer()`
- `copy_from_user()`
- `debugfs_remove_recursive()`
