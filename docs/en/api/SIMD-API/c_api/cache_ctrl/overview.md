# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T09:55:23.325Z pushedAt=2026-11-18T08:15:34.205Z nl=2 -->

## Cache Control Overview

### Cache Types

The main purpose of a cache is to provide a high-speed buffer between the transfer unit or Scalar unit and external storage, thereby reducing data access latency and improving bandwidth utilization. Typically, frequently accessed data is written to the cache, and the transfer unit or Scalar unit reads data from the cache first during execution. When a cache miss occurs, data is loaded from external storage and updated into the cache.

[Table 1](#table_cache_type) shows the functions of different cache types, where the Data Cache (DCache) and Instruction Cache (ICache) are used to cache data and instructions, respectively. The cache types supported vary by product, as follows:
<!-- npu="950" id1 -->
Taking [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) as an example, Figure 1 shows the positional relationships of the five cache types supported in the AI Core (L2 Cache, DCache, ICache, SIMT DCache, and NDDMA Cache) within the hardware architecture.

**Figure 1** Diagram of the positional relationships of the five cache types in the AI Core

![](../../../figures/npu_3510_hw_arch_cache.png "atlas_950_cache_architecture_diagram")
<!-- end id1 -->
<!-- npu="A3,910b" id2 -->
Taking [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch) as an example, Figure 2 shows the positional relationships of the three cache types supported in the AI Core (L2 Cache, DCache, and ICache) within the hardware architecture.

**Figure 2** Diagram of the positional relationships of the three cache types in the AI Core

![](../../../figures/atlas_a2_a3_cache_architecture_diagram.png "atlas_a2_a3_cache_architecture_diagram")
<!-- end id2 -->

**Table 1** Cache types and function descriptions<a name="table_cache_type"></a>

| Cache Type | Description |
|------------|------|
| L2 Cache | As a level-2 cache, L2 Cache is dedicated to storing frequently accessed data and instructions to reduce reads and writes to GM.<br>&bull;When reading GM through the MTE2 unit, data is preferentially read from L2 Cache; when L2 Cache misses, data is loaded from GM and updated into L2 Cache.<br>&bull;When reading GM through the Scalar unit, data is preferentially read from DCache; when DCache misses, data is read from L2 Cache. When L2 Cache also misses, data is loaded from GM and updated into both L2 Cache and DCache.<br>&bull;When reading GM instructions through the Scalar unit, instructions are preferentially read from ICache; when ICache misses, instructions are read from L2 Cache. When L2 Cache also misses, instructions are loaded from GM and updated into both L2 Cache and ICache. |
| DCache | DCache is used to cache data segments that the Scalar unit may repeatedly access in the near future.<br>When reading GM through the Scalar unit, data is preferentially read from DCache; when DCache misses, data is read from L2 Cache. When L2 Cache also misses, data is loaded from GM and updated into both L2 Cache and DCache. |
| ICache | ICache is used to cache instructions that the Scalar unit has recently used or frequently uses.<br>When reading GM instructions through the Scalar unit, instructions are preferentially read from ICache; when ICache misses, instructions are read from L2 Cache. When L2 Cache also misses, instructions are loaded from GM and updated into both L2 Cache and ICache. |
| SIMT DCache | SIMT access to GM must be relayed through SIMT DCache. The data flow goes from GM to SIMT DCache, and then from SIMT DCache to SIMT registers. SIMT DCache acts as an intermediate buffer layer, reducing the number of direct accesses to GM. |
| NDDMA Cache | NDDMA Cache is used to cache data that has recently been or is about to be moved by the [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md) interface. |

For hardware specifications such as the cache line size, cache size, and location of each cache, see the [Cache Coherence Cache Unit Information](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#cache_unit_info) document.

### Cache Operations and Usage Scenarios

Table 2 describes the four basic cache operations and their core functions.

**Table 2** Cache operations and function descriptions

| Operation Name | Description |
|---------|---------|
| Prefetch | The hardware automatically loads the data expected to be accessed into the cache in advance based on the access pattern, improving subsequent access speed. |
| Preload | The software actively loads specified data into the cache through explicit instructions, preparing for upcoming read and write operations. |
| Invalid | &bull;Marks the Cache Lines (the smallest operation unit of the cache) in the specified address range as "invalid", removing them from the cache.<br>&bull;Ensures that the next access to these memory addresses reloads data from GM instead of using possibly stale cached data.<br>&bull;Note: The Invalid operation does not check whether a cache line is "dirty" (indicating that the data has been modified but not yet written back to GM). If dirty data exists, it will be discarded directly. |
| Clean | Writes the modified data (dirty data) in the cache back to GM to avoid data loss. The Clean operation does not mark the cache line as "invalid"; the cache line remains in a valid state. |

### Cache Write Policy and Cache Coherency Issues

When writing data to GM with cache enabled, there are two cache write policies: Write-Back and Write-Through.

When the Write-Back policy is used for writing data to GM, the working principle is as follows:

- The developer expects to write data to GM (that is, expects the data to be written directly to GM), but the actual hardware behavior is:
    - When data is modified, it is updated only in the cache.
    - The data in the cache is marked as "dirty".
    - When the cache line is replaced (or when a Clean operation is explicitly performed by software), the data is written back to GM. The timing of cache line replacement is determined by the hardware.

- When replacing cache line data:
    - When cache space is insufficient and data needs to be replaced, check whether the data to be replaced is "dirty".
    - If it is "dirty" data, write it back to GM first, and then replace the cache line out of the cache.

When writing data to GM, if the write-through policy is adopted, its working principle is as follows:

- The developer expects to write data to GM (that is, expects the data to be written directly to GM), while the actual hardware behavior is:
    - When data is modified, the data is updated in both the cache and GM immediately.
    - The cache line does not need to be marked as "dirty".

- When replacing cache line data:

    Since every data write synchronously updates GM, the data in the cache does not have a "dirty" state, and there is no need to additionally write back to GM during replacement.

Under the delayed write-back policy, the modified data is updated only in the local cache and is not immediately synchronized to GM. When other cores read the same address in GM, they will obtain stale data, resulting in inconsistent cache data among different cores.
Cache coherence is the core mechanism for ensuring data correctness in multi-core systems. In simple terms, when multiple cores each have their own independent cache, they may cache the same data simultaneously. If one core modifies the data in its cache, the cache copies of other cores become "stale". If they are not synchronized in time, data inconsistency among multiple cores will occur, which in turn leads to computation errors.

<!-- npu="950,A3,910b" id3 -->
The following describes whether each type of cache in different products needs to consider data inconsistency among multiple cores:
<!-- npu="950" id4 -->
- For [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the situation of the five types of cache supported by this product regarding data consistency among multiple cores is as follows:
    - L2 Cache: a cache shared among multiple cores, so there is no need to consider the problem of data inconsistency among multiple cores.
    - DCache: an independent cache for each core, so data consistency across cores must be considered.
    - ICache: read-only, so data inconsistency across cores does not need to be considered.
    - NDDMA Cache: an independent cache for each core, so data consistency across cores must be considered.
    - SIMT DCache: in SIMT, data is written to GM immediately when written, so there is no consistency issue; when reading data from GM, each core has an independent cache, so data consistency across cores must be considered.
<!-- end id4 -->
<!-- npu="A3,910b" id5 -->
- For [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), the data consistency across cores for the three supported cache types is as follows:
    - L2 Cache: a cache shared across cores, so data inconsistency across cores does not need to be considered.
    - DCache: an independent cache for each core, so data consistency across cores must be considered.
    - ICache: read-only, so data inconsistency across cores does not need to be considered.
<!-- end id5 -->
<!-- end id3 -->

For a more complete introduction to cache write policies and cache coherence, see the [cache coherence](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md) document.

## API List

Table 3 summarizes the cache control related interfaces and their function descriptions.

**Table 3** Cache control interfaces

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_datacache_preload](asc_datacache_preload.md) | Preloads data from the specific GM address where the source address resides into the Data Cache. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_dcci](asc_dcci.md) | Used to flush the cache to ensure cache consistency. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_dci](asc_dci.md) | Invalidates the data cache, making the entire data cache invalid. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_get_icache_preload_status](asc_get_icache_preload_status.md) | Obtains the status of ICache preload. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_get_scalar_load_cache_mode](asc_get_scalar_load_cache_mode.md) | Obtains the L2 Cache management policy of the scalar load path. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_get_scalar_store_cache_mode](asc_get_scalar_store_cache_mode.md) | Obtains the L2 Cache management policy of the scalar store path. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_icache_preload](asc_icache_preload.md) | Preloads data from the GM address where the instruction resides into the corresponding cacheline. | `c_api/cache_ctrl/cache_ctrl.h` |
| [asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md) | Configures the L2 Cache management policy when the scalar unit accesses Global Memory. | `c_api/cache_ctrl/cache_ctrl.h` |
