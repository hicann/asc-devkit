# Introduction to Memory Access Functions

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-10-27T03:36:42.248Z pushedAt=2026-10-27T09:21:55.701Z nl=2 -->

SIMT programming typically requires a large number of threads to access Global Memory in parallel. Memory access functions are used to explicitly control the cache path and cache consistency maintenance method when accessing Global Memory, helping developers select the appropriate load, store, or cache invalidation API based on data reuse characteristics, write-back methods, and cross-core visibility requirements.

Based on general SIMT programming experience, when threads within the same warp access contiguous and aligned Global Memory addresses, it is usually more conducive to forming efficient memory access. When memory access addresses are scattered, unaligned, or frequently cross cache lines, the number of memory access transactions may increase and throughput may decrease. Developers should select memory access APIs based on data layout, access patterns, and cache reuse.

## Basic Concepts

| Concept | Description |
| --- | --- |
| Global Memory | The global memory space accessible to all threads, and the primary access target of memory access functions. |
| L2 Cache | The cache level on the Global Memory access path, which can cache data read from or written to Global Memory. |
| Data Cache | The cache space used when SIMT threads access Global Memory, suitable for data access with local reuse. |
| Cache Line | The basic granularity for cache maintenance and invalidation. The `asc_dcci_single` API is used to invalidate the Cache Line where the specified address resides. |

## API List

| Category | Interface Name | Description |
| --- | --- | --- |
| Load function | [asc\_ldcg](asc_ldcg.md) | Loads data from L2 Cache; on a miss, reads from Global Memory and caches to L2 Cache. |
| Load function | [asc\_ldca](asc_ldca.md) | Loads data from Data Cache first; on a miss, attempts to read from L2 Cache or Global Memory, and caches to L2 Cache and Data Cache. |
| Store function | [asc\_stcg](asc_stcg.md) | Stores data to Global Memory and caches to L2 Cache, without caching to Data Cache. |
| Store function | [asc\_stwt](asc_stwt.md) | Stores data to Global Memory and caches to Data Cache and L2 Cache. |
| Cache invalidation function | [asc\_dcci\_single](asc_dcci_single.md) | Invalidates the Cache Line where the specified address resides to ensure cache consistency for subsequent reads. |
| Cache invalidation function | [asc\_dcci\_entire](asc_dcci_entire.md) | Invalidates the entire Data Cache within the core to ensure cache consistency for subsequent reads. This API incurs high overhead and should be used with caution in performance-sensitive scenarios. |
| No-operation function | [asc\_nop](asc_nop.md) | Generates a no-operation instruction that occupies the current thread for 15 clock cycles without performing any actual computation or memory access. |

## Interface Selection

### Load Path Selection

When data has local reuse and may be accessed again by the current thread or the current core, consider using `asc_ldca` so that the data enters both the Data Cache and the L2 Cache. If the data mainly relies on the L2 Cache, or if you want to avoid writing to the Data Cache, use `asc_ldcg`.

```cpp
float value = asc_ldca(input + idx);
```

### Store Path Selection

`asc_stcg` writes data to Global Memory and caches it in L2 Cache, but not in Data Cache; `asc_stwt` writes data to Global Memory and caches it in both Data Cache and L2 Cache. If the written data is likely to be read again soon by the current core, you can choose `asc_stwt` based on the access reuse pattern; if you do not want to pollute Data Cache, consider `asc_stcg`.

```cpp
asc_stcg(output + idx, value);
```

### Cache Consistency Maintenance

In a SIMT program, written data is immediately written to Global Memory, making it visible to other cores. When data read from Global Memory may have been modified by other cores, you can use the DCCI APIs to invalidate the Data Cache and obtain the latest data from Global Memory.

`asc_dcci_single` invalidates only the Cache Line where the specified address resides, making it suitable for small-range invalidation scenarios such as polling flag bits. `asc_dcci_entire` invalidates the entire Data Cache, which incurs higher overhead and should be used only when full invalidation is truly required.

```cpp
while (flag[0] != 1U) {
    asc_dcci_single(static_cast<void *>(flag));
}
```

## Usage Recommendations

-   Prioritize having adjacent threads within the same warp access contiguous, aligned Global Memory addresses to improve memory access efficiency.
-   For contiguous and aligned data, select vector types such as `float2`, `float4`, `int2`, and `int4` based on the data types supported by the API to reduce the number of memory access instructions per thread.
-   Select the load and store paths based on data reuse to avoid unnecessarily occupying the Data Cache.
-   When passing data and flags across thread blocks or cores through Global Memory, design the synchronization protocol in combination with [Synchronization and Memory Fence](../sync_and_memory_fence/sync_and_memory_fence.md), [Atomic Operations](../atomic_operations/atomic_operations.md), and DCCI APIs.
-   DCCI APIs are used to handle cache coherence issues. They are not equivalent to thread synchronization APIs, nor can they replace atomic operations.
-   `asc_nop` is mainly used to avoid potential deadlock issues that may occur when multiple threads read from and write to the same address.
