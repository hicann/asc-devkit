# Cache Control

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:21:11.769Z pushedAt=2026-11-19T05:54:06.629Z nl=2 -->

The Tensor API provides data cache control capabilities:

- [datacache_clean_and_invalid](datacache_clean_and_invalid.md): Cleans and invalidates cache lines in the Data Cache by target domain.
- [datacache_clean](datacache_clean.md): Marks the entire Data Cache as invalid without writing back dirty data.
- [datacache_preload](datacache_preload.md): Prefetches one cache line of data from Global Memory to the DCache.
- [datacache_clean_and_invalid_line](datacache_clean_and_invalid_line.md): Cleans and invalidates the Data Cache line at the Global Memory or Unified Buffer address.
- [icache_preload](icache_preload.md): Prefetches instructions from Global Memory to the ICache.
- [get_icache_preload_status](get_icache_preload_status.md): Obtains the ICache prefetch status.
- [set_scalar_cache_mode](set_scalar_cache_mode.md): Configures the L2 Cache management policy for the scalar load/store path.
- [get_scalar_load_cache_mode](get_scalar_load_cache_mode.md): Reads the L2 Cache management policy for the scalar load path.
- [get_scalar_store_cache_mode](get_scalar_store_cache_mode.md): Reads the L2 Cache management policy for the scalar store path.
