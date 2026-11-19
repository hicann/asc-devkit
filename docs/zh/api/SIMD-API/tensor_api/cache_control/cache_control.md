# 缓存控制

Tensor API提供数据缓存控制能力：

- [datacache_clean_and_invalid](datacache_clean_and_invalid.md)：按目标域清理并失效Data Cache中的缓存行。
- [datacache_clean](datacache_clean.md)：将整个Data Cache标记为无效，不回写脏数据。
- [datacache_preload](datacache_preload.md)：从Global Memory预取一个Cache Line的数据到DCache。
- [datacache_clean_and_invalid_line](datacache_clean_and_invalid_line.md)：清理并失效Global Memory或Unified Buffer地址所在的Data Cache缓存行。
- [enable_scalar_ub_dcache](enable_scalar_ub_dcache.md)：配置标量单元访问Unified Buffer时经过DCache。
- [disable_scalar_ub_dcache](disable_scalar_ub_dcache.md)：配置标量单元访问Unified Buffer时直接访问UB。
- [icache_preload](icache_preload.md)：从Global Memory预取指令到ICache。
- [get_icache_preload_status](get_icache_preload_status.md)：获取ICache预取状态。
- [set_scalar_cache_mode](set_scalar_cache_mode.md)：配置标量load/store路径的L2 Cache管理策略。
- [get_scalar_load_cache_mode](get_scalar_load_cache_mode.md)：读取标量load路径的L2 Cache管理策略。
- [get_scalar_store_cache_mode](get_scalar_store_cache_mode.md)：读取标量store路径的L2 Cache管理策略。
