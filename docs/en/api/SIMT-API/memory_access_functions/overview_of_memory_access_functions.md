# Introduction to Memory Access Functions

<!-- md-trans-meta sourceCommit=e1d69e702c9c347bfbdba7ca2ed786dab14f1ccf translatedAt=2026-08-28T07:16:47.266Z -->

> **Prerequisites**: This document involves SIMT thread organization concepts such as Grid, Thread Block, Thread, and Warp. It is recommended that you first read [Thread Architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/programming_model/ai_core_simd_programming/thread_architecture.md) in the programming guide to understand the relevant prerequisites.

SIMT programming typically requires a large number of threads to access Global Memory in parallel. Memory access functions are used to explicitly control the cache path and cache consistency maintenance method when accessing Global Memory, helping developers select the appropriate load, store, or cache invalidation interface based on data reuse characteristics, write-back methods, and cross-core visibility requirements.

Based on general SIMT programming experience, when threads within the same warp access contiguous and aligned Global Memory addresses, it is usually more conducive to forming efficient memory access. When memory access addresses are scattered, unaligned, or frequently cross cache lines, the number of memory access transactions may increase and throughput may decrease. Developers should select memory access interfaces based on data layout, access patterns, and cache reuse.

## Basic Concepts

| Concept | Description |
| --- | --- |
| Global Memory | The global memory space accessible to all threads, and the primary access target of the memory access functions. |
| L2 Cache | The cache level on the Global Memory access path, which can cache data read from or written to Global Memory. |
| Data Cache | The cache space used when SIMT threads access Global Memory, suitable for data access with local reuse. |
| Cache Line | The basic granularity for cache maintenance and invalidation. The `asc_dcci_single` interface is used to invalidate the Cache Line where the specified address resides. |

## API List

| Category | Interface | Description |
| --- | --- | --- |
| Load function | [asc\_ldcg](asc_ldcg.md) | Loads data from the L2 Cache; on a miss, reads from Global Memory and caches it into the L2 Cache. |
| Load function | [asc\_ldca](asc_ldca.md) | Loads data from the Data Cache first; on a miss, attempts to read from the L2 Cache or Global Memory and caches it into both the L2 Cache and the Data Cache. |
| Store function | [asc\_stcg](asc_stcg.md) | Stores data to Global Memory and caches it into the L2 Cache, but not into the Data Cache. |
| Store function | [asc\_stwt](asc_stwt.md) | Stores data to Global Memory and caches it into both the Data Cache and the L2 Cache. |
| Cache invalidation function | [asc\_dcci\_single](asc_dcci_single.md) | Invalidates the Cache Line containing the specified address to ensure cache consistency for subsequent reads. |
| Cache invalidation function | [asc\_dcci\_entire](asc_dcci_entire.md) | Invalidates the entire Data Cache within the core to ensure cache consistency for subsequent reads. This interface incurs high overhead and should be used with caution in performance-sensitive scenarios. |
| No-op function | [asc\_nop](asc_nop.md) | Occupies the current thread for 15 clock cycles without performing any actual computation or memory access. |

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

In a SIMT program, written data is immediately written to Global Memory, making it visible to other cores. When data read from Global Memory may have been modified by other cores, you can use the DCCI-class interfaces to invalidate the Data Cache and obtain the latest data from Global Memory.

`asc_dcci_single` invalidates only the Cache Line where the specified address resides, making it suitable for small-range invalidation scenarios such as polling flag bits. `asc_dcci_entire` invalidates the entire Data Cache, which incurs higher overhead and should be used only when full invalidation is truly required.

```cpp
while (flag[0] != 1U) {
    asc_dcci_single(static_cast<void *>(flag));
}
```

## Usage Recommendations

-   Select the load and store paths based on data reuse to avoid unnecessarily occupying the Data Cache.
-   When passing data and flags across Thread Blocks or across cores through Global Memory, design the synchronization protocol by combining [synchronization and memory barriers](../sync_and_memory_fence/sync_and_memory_fence.md), [atomic operations](../atomic_operations/atomic_operations_149.md), and DCCI-class interfaces.
-   DCCI-class interfaces are used to handle cache coherence issues. They are not equivalent to thread synchronization interfaces, nor can they replace atomic operations.
-   `asc_nop` is mainly used to avoid potential deadlock issues that may occur when multiple threads read from and write to the same address.

For memory access efficiency optimization methods such as memory access continuity, alignment, and vector types, see the performance optimization guide [Memory Access Coalescing](../../../guide/operator_practice/simd_operator_optimization/memory_access/memory_access.md) and [Using Short Vector Types to Improve Efficiency](../../../guide/operator_practice/simt_operator_optimization/memory_access/short_vector_types_for_efficiency_optimization.md).
