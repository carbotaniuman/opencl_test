void kernel test(global atomic_int* status, global int* res) {
    int id = get_global_id(0);

    int data;
    while ((data = atomic_load_explicit(status, memory_order_acquire, memory_scope_device)) != 0xBEEF) {}

    res[id * 2 + 0] = data;
    res[id * 2 + 1] = 5555;
}
