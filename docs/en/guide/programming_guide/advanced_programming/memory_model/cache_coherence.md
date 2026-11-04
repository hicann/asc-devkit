# Cache Coherence

<!-- md-trans-meta sourceCommit=5e55c38a4e567465079c4ca7387d07ab9e6347c5 translatedAt=2026-10-23T14:28:00.077Z pushedAt=2026-11-03T07:05:13.430Z nl=2 -->

This document describes common cache coherence issues in NPU programming and explains the data consistency among Data Cache (DCache), Instruction Cache (ICache), SIMT DCache, and Global Memory (GM). For the hardware architecture of the chip, see [NPU Architecture Version 2201](../hardware_implementation/architecture_spec/npu_arch_2201.md) and [NPU Architecture Version 3510](../hardware_implementation/architecture_spec/npu_arch_3510.md).

## Basic Concepts of Cache Coherence

### Cache, Cache Line, and Cache Copies

A cache is used to cache data or instructions that may be accessed repeatedly later, reducing the overhead of accessing GM. A single cache load usually does not load only the single byte or single variable being accessed. Instead, it loads a contiguous segment of data at the address location into the cache as a whole, and this whole is called a cache line.

A cache line is the minimum operation granularity for cache loading, invalidation, and writeback. For example, when accessing GM memory, adjacent data within the length of one cache line in GM is loaded into the L2 cache, using spatial locality to improve access efficiency. A cache line is the minimum granularity at which hardware actually moves in, marks, replaces, and writes back cached data, and it directly affects performance and coherence. <!-- npu="950" id1 -->[NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) supports L2 Cache, DCache, ICache, SIMT DCache, and NDDMA Cache; <!-- end id1 -->[NPU Architecture Version 2201](../../language_extension/simd_builtin_keywords.md) supports L2 Cache, DCache, and ICache. The general cache unit information discussed in this document is as follows:<a name="cache_unit_info"></a>

<!-- npu="950" id15 -->
**NPU Architecture Version 3510 Cache Unit Information**

| Storage Unit | Location | Data Consistency Description | [NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) Cache Line Size | [NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) Cache Size |
| --- | --- | --- | --- | --- |
| L2 Cache | Off-core shared cache | Shared by multiple cores; cache copy inconsistency among multiple cores is not considered | 128B/512B | 128MB |
| AIC ICache | Scalar instruction fetch cache | Read-only; data inconsistency among multiple cores is not considered | 128B | 32KB |
| AIV ICache | Scalar instruction fetch cache | Read-only; data inconsistency among multiple cores is not considered | 128B | 16KB |
| AIC DCache | Scalar data cache | Independent cache per core; coherence issues among multiple cores need to be considered | 64B | 32KB |
| AIV DCache | Scalar data cache | Independent cache per core; coherence issues among multiple cores need to be considered | 64B | 32KB |
| SIMT DCache | SIMT data cache | Independent cache per core; coherence issues among multiple cores need to be considered | 128B | 32KB~128KB |

<!-- end id15 -->

<!-- npu="A3,910b" id20 -->
**NPU Architecture Version 2201 Cache Unit Information**

| Storage Unit | Location | Data Consistency Description | [NPU Architecture Version 2201](../../language_extension/simd_builtin_keywords.md) Cache Line Size | [NPU Architecture Version 2201](../../language_extension/simd_builtin_keywords.md) Cache Size |
| --- | --- | --- | --- | --- |
| L2 Cache | Off-core shared cache | Shared by multiple cores; cache copy inconsistency among multiple cores is not considered | 512B | 192MB |
| AIC ICache | Scalar instruction fetch cache | Read-only; data inconsistency among multiple cores is not considered | 128B | 32KB |
| AIV ICache | Scalar instruction fetch cache | Read-only; data inconsistency among multiple cores is not considered | 128B | 16KB |
| AIC DCache | Scalar data cache | Independent cache per core; coherence issues among multiple cores need to be considered | 64B | 16KB |
| AIV DCache | Scalar data cache | Independent cache per core; coherence issues among multiple cores need to be considered | 64B | 32KB |

<!-- end id20 -->

The same piece of data may exist simultaneously in multiple locations such as GM, L2 Cache, DCache<!-- npu="950" id2 -->, and SIMT DCache<!-- end id2 -->. For caches independently maintained by each core, the same address or the same cache line may form different copies in different cores:

- A core has already modified the data in its own cache, but GM or the caches of other cores still retain the old value;
- A core has already cached an old value before reading, and even if GM is later updated through another path, subsequent reads may still continue to hit the old copy.

This kind of inconsistent copies is the cache coherence issue that this document focuses on.

### Clean, Dirty, Invalid, Write Policy, and DCCI

Data in the Cache can be understood as the following states based on its relationship with the next-level storage:

- **Clean**: The data in the Cache Line is consistent with the data in the next-level storage. Discarding the Cache Line directly does not lose any updates.
- **Dirty**: The data in the Cache Line has been modified by the current Cache but has not yet been written back to the next-level storage. At this point, the data in the Cache is inconsistent with the data in the next-level storage. If another path reads GM directly, it may read a stale value.
- **Invalid**: The Cache Line is invalid. Subsequent accesses cannot continue to use the stale data in the Cache Line and must reload it from the next-level storage.

Data Cache Clean and Invalid ([DCCI](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)) is used to handle Cache copies. The Clean operation writes Dirty Cache Lines back to the target storage, and the Invalid operation invalidates Cache Lines to prevent subsequent accesses from continuing to hit stale copies.

In actual use, you can select different DCCI APIs based on the coherence target: use [`asc_dcci_single`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md) to flush a single Cache Line at a specified address; use [`asc_dcci_entire_out`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md) to flush the coherence target for GM; for atomic-related targets, select APIs such as [`asc_dcci_entire_atomic`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md) based on product support.

For scenarios where you need to wait for the writeback triggered by DCCI to actually complete, you also need to use the Data Sync Barrier ([DSB](../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md)) to wait for preceding memory accesses to complete. DSB is only used to block the execution of subsequent instructions until the preceding memory access instructions finish. The parameter can be `DSB_ALL`, `DSB_DDR`, or `DSB_UB`.

The Cache write policy determines whether a write operation creates a Dirty copy. Common policies include write-through and write-back:

- **Write-through**: The write operation updates the Cache copy and simultaneously writes the data out to the next-level storage. The write-through path usually does not leave Dirty copies that need to be cleaned back to the next-level storage later, but whether the write operation has completed still needs to be confirmed through synchronization mechanisms such as DSB. If the read end has already cached a stale copy, Invalid still needs to be executed on the read-end path.
- **Write-back**: A write operation first updates the Cache copy and marks the Cache Line as Dirty; the next-level storage is not updated until occasions such as DCCI Clean or Cache Line replacement. The write-back path is more likely to cause the problem that "the write end has already executed Store, but GM or Unified Buffer (UB) still holds the old value".


### Difference Between Cache Coherence and Memory Consistency

Cache coherence focuses on whether the values of the same address or the same cache line are consistent across different cache copies. For example, the DCache of a core has already modified `A` to a new value, but GM still holds the old value; or GM has been updated through another path, but the DCache of the current core still retains the old copy. In such cases, the key question is whether the dirty copy needs to be written back, the old copy needs to be invalidated, or the cache needs to be bypassed for access.

Memory consistency focuses on whether the read/write order among different execution units, threads, or cores meets program expectations. For example, a producer writes data first and then writes a flag; after the consumer observes the flag, can it always read the data as expected? Synchronization interfaces and memory fences mainly address execution order and access completion issues, while DCCI mainly addresses cache copy writeback and invalidation issues. In actual programming, the two types of issues often occur simultaneously, but this document focuses on the data consistency between cache copies and GM or UB.

The following uses DCache as an example to illustrate cache coherence issues. `A` and `B` are located in the same 64B DCache cache line in GM:

```text
Initial state:
Data distribution over a 64B address range in GM [A=0, B=0, ...]

Time    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.LOAD A
          // Core 0 DCache caches the entire cache line:
          // [A=0, B=0, ...]

T1                                                Scalar.LOAD B
                                                  // Core 1 DCache caches the entire cache line:
                                                  // [A=0, B=0, ...]

T2        Scalar.STORE A = 1
          // Core 0 DCache becomes:
          // [A=1, B=0, ...]

T3                                                Scalar.STORE B = 2
                                                  // Core 1 DCache becomes:
                                                  // [A=0, B=2, ...]
```

Although the two cores modify different fields, each DCache holds a copy of the entire cache line. When writing back to GM later, if the writeback is performed in units of the entire cache line, different results may occur:

```text
Case 1: Core 0 writes back first, Core 1 writes back later

Time    Core 0                                  Core 1
======    ====================================    ====================================
T4        Scalar.DCCI
          // Core 0 writes back at cache line granularity:
          // GM Cache Line = [A=1, B=0, ...]

T5                                                Scalar.DCCI
                                                  // Core 1 writes back at cache line granularity:
                                                  // GM Cache Line = [A=0, B=2, ...]

Case 2: Core 1 writes back first, Core 0 writes back later

Time    Core 0                                  Core 1
======    ====================================    ====================================
T4                                                Scalar.DCCI
                                                  // Core 1 writes back at cache line granularity:
                                                  // GM Cache Line = [A=0, B=2, ...]

T5        Scalar.DCCI
          // Core 0 writes back at cache line granularity:
          // GM Cache Line = [A=1, B=0, ...]
```

The expected result is: GM Cache Line = [A=1, B=2, ...]

However, because each core has its own cached data, and each core modifies different fields of the same Cache Line on its own old cached copy, the actual possible results are:
- Case 1: Core 0 writes back first, then Core 1 writes back, and the final GM Cache Line = [A=0, B=2, ...]
- Case 2: Core 1 writes back first, then Core 0 writes back, and the final GM Cache Line = [A=1, B=0, ...]

In this example, because cache coherence was not ensured, the expected `GM Cache Line = [A=1, B=2, ...]` was not obtained.

## Cache Coherence

### Cache Coherence Between DCache and GM

Cache coherence between DCache and GM is the most common type of cache coherence issue in NPU programming. When the Scalar performs normal read/write operations on GM, the data passes through the DCache of the current core, and each AIC/AIV core has an independent DCache used to cache data in GM. After a core modifies the data corresponding to a GM address through the DCache, the new value may first remain in the DCache of that core, and the copies in GM and in the DCaches of other cores are not automatically updated.

When multiple cores or multiple access paths operate on the same segment of GM data, the inconsistency between DCache and GM typically comes from two types of copies:

- Dirty copy on the write end: The producer has already written the data, but the new value still remains in the producer's DCache, and GM has not been updated yet.
- Old copy on the read end: The consumer has previously cached the same address or the same Cache Line, and subsequent reads continue to hit the old value in the consumer's own DCache.

The normal read/write behavior of the Scalar on GM can be summarized as follows:

- After the Scalar reads GM, the data enters the DCache. Subsequent reads of data within the same Cache Line may be read directly from the DCache without accessing GM again.
- When the Scalar writes GM, it only modifies the Cache Line in the DCache and marks that Cache Line as Dirty. Dirty indicates that the Cache Line data in the DCache is inconsistent with the data in GM, but the Dirty data is not written back to GM immediately. Instead, it is written back to GM only when the user manually flushes it or when the data segment is replaced.

Therefore, the producer side must pay attention to whether the Dirty data has been written back to GM, and the consumer side must pay attention to whether an old copy is still retained in its own DCache. DCCI is responsible for handling "whether the copy is written back or invalidated", while DSB and pipeline synchronization are only responsible for handling "whether subsequent accesses wait for the completion of preceding accesses".

#### Cache Coherence Brought by Scalar Access to GM

When Scalar performs normal read/write on GM, the execution of `Scalar.STORE` does not mean that GM has been rewritten. The following uses two cores accessing the same GM address `A` to illustrate the producer-side Dirty copy and consumer-side old copy issues. The time points such as `T0` and `T1` in the example only indicate the logical order of the code.

```text
Initial state:
GM[A] = 0
Core 0 DCache does not cache A
Core 1 DCache does not cache A

Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A] may still be 0

T1                                                // If Scalar.LOAD A is executed at this time -> 0
                                                  // Core 1 may load the old value 0 from GM

T2        Scalar.DCCI A
T3        Scalar.DSB
          // Dirty Cache Line writeback completed, GM[A] = 1

T4                                                Scalar.LOAD A -> 1
                                                  // If Core 1 has no old DCache copy, it can read the new value 1
```

If Core 1 has not cached `A` before T3, executing `Scalar.LOAD A` at T4 can obtain the new value `1` from GM. If Core 1 has cached the old value, the old copy on the read end must be handled further.

If the read end has already cached the same address in its own DCache, the old copy must also be considered. In the following example, the consumer refreshes the Cache Line where the specified GM address is located before reading data using DCCI:

```text
Initial state:
GM[A] = 0
Core 0 DCache does not cache A
Core 1 DCache does not cache A

Time point    Core 0                                  Core 1
======    ====================================    ====================================
T0        Scalar.LOAD A -> 0                      Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0                 // Core 1 DCache[A] = 0

T1        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A] may still be 0

T2        Scalar.DCCI A

T3        Scalar.DSB
          // Dirty Cache Line writeback is complete, GM[A] = 1

T4                                                Scalar.LOAD A -> 0
                                                  // Core 1 DCache[A] = 0, Core 1 may continue hitting its own old DCache copy

T5                                                Scalar.DCCI A
                                                  // Invalidate the old cache copy in Core 1

T6                                                Scalar.LOAD A -> 1
                                                  // Core 1 DCache[A] = 1, Core 1 fetches the new value from GM again
```

The two examples above correspond to the Dirty copy on the write end and the old copy on the read end, respectively. The cache handling principles for the Scalar path are as follows:

- After a producer writes to GM through Scalar, if a consumer needs to read the data from GM or another path, the producer must execute DCCI (for example, `asc_dcci_single`) to write the Dirty data back to GM.
- If a consumer is concerned that its own core DCache contains an old copy, it must execute DCCI before reading to invalidate the old copy.
- If the algorithm itself does not require DCache caching, you can use [ReadGmBypassDCache](<../../../../api/SIMD-API/basic_api/scalar_compute/ReadGmBypassDCache_ISASI.md>) and [WriteGmBypassDCache](<../../../../api/SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md>) to read and write GM directly.

Another type of problem comes from the Cache Line granularity. A normal Scalar write is subsequently written back in units of 64B Cache Lines. Even if multiple cores write different fields within the same Cache Line, they may overwrite each other's results when each writes back the entire Cache Line. In such scenarios, you can reduce the risk by ensuring that the addresses operated on by different cores are at least one Cache Line size apart, or by using ReadGmBypassDCache and WriteGmBypassDCache.

<!-- npu="950" id3 -->
#### DCache Cache Coherence Caused by Atomic Access

[NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) supports Scalar atomic operations. A Scalar atomic operation can be regarded as a read-modify-write event performed on a GM address, but the Scalar atomic operation bypasses the DCache, which causes cache coherence issues. For example, the C API [asc_atomic_add](../../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md) performs an atomic add operation on a specified GM address. An atomic operation guarantees that this read-modify-write is not interrupted by other atomic updates, but it does not automatically clean the old copy or Dirty copy of the same address in the DCache.

Therefore, if the same GM address was previously accessed through the normal Scalar path, you need to analyze the coherence issue between the atomic event and the DCache event:

- If the normal Scalar write occurs first, the data may still remain in the DCache. If the subsequent atomic operation depends on this write, you need to first write back the relevant DCache Line to the GM.
- If the atomic operation occurs first, the DCache of this core may still retain the old copy. When the same address is subsequently read through the normal Scalar path, you need to first invalidate the old copy to avoid reading the value before the atomic operation.

The following uses address `A` and asc_atomic_add to illustrate the problem when Atomic and DCache are used together:

```text
Case 1: Normal Scalar write first, Atomic later
Initial state:
GM[A] = 0
A is not cached in the DCache of Core 0

Time    Core 0
======    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A] may still be 0

T1        asc_atomic_add(A, 1)
          // Atomic bypasses DCache to access GM
          // If Atomic starts accumulating from the old GM value 0, GM[A] may become 1 instead of the expected 2
```

The key point above is not that `asc_atomic_add` itself is non-atomic, but that the GM value seen by the atomic operation does not include the normal Scalar write that previously stayed in the DCache.

Conversely, if `asc_atomic_add(A, 1)` has already updated GM to 1, while the DCache of this core still retains the old copy `A == 0`, a subsequent normal Scalar read may also continue to read the old copy.
```text
Case 2: Atomic first, normal Scalar read later
Initial state:
GM[A] = 0
Core 0 DCache does not cache A

Time    Core 0
======    ====================================
T0        Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0, Clean

T1        asc_atomic_add(A, 1)
          // Atomic bypasses DCache to update GM
          // GM[A] = 1
          // Core 0 DCache[A] may still be 0

T2        Scalar.LOAD A -> 0
          // A normal Scalar read may continue to hit the old DCache copy in this core
```

To avoid the preceding problems, Atomic access usually needs to handle cache coherence issues. The correct processing flow should be:
- Case 1: When a normal Scalar write comes first and Atomic comes later, write the DCache back to GM first, and then execute the Atomic operation.
- Case 2: When Atomic comes first and a normal Scalar read comes later, invalidate the old DCache copy first, and then execute the normal Scalar read.

The following shows the correct flow examples:

```text
Case 1: Correct processing flow when a normal Scalar write comes first and Atomic comes later
Initial state:
GM[A] = 0
Core 0 DCache does not cache A

Time      Core 0
======    ====================================
T0        Scalar.STORE A = 1
          // Core 0 DCache[A] = 1, Dirty
          // GM[A] may still be 0

T1        Scalar.DCCI A
T2        Scalar.DSB
          // Dirty Cache Line writeback complete, GM[A] = 1

T3        asc_atomic_add(A, 1)
          // Atomic starts accumulating from GM[A] = 1
          // GM[A] = 2

T4        Scalar.DSB
          // Wait for the GM access generated by Atomic to complete
```

```text
Case 2: Correct processing flow when Atomic comes first and a normal Scalar read comes later
Initial state:
GM[A] = 0
Core 0 DCache does not cache A

Time point    Core 0
======    ====================================
T0        Scalar.LOAD A -> 0
          // Core 0 DCache[A] = 0, Clean

T1        asc_atomic_add(A, 1)
T2        Scalar.DSB
          // Wait for the GM access generated by Atomic to complete, GM[A] = 1

T3        Scalar.DCCI A
          // Invalidate the old Cache copy of Core 0

T4        Scalar.LOAD A -> 1
          // Re-obtain the new value after the Atomic update from GM.
```

<!-- end id3 -->

#### Software Maintenance of DCache Coherence for Scalar Access to GM

This section describes how to maintain DCache coherence caused by Scalar access to GM at the software level. The hardware only maintains cache coherence in scenarios where both the consumer and the producer come from Scalar DCache operations on the same core. It does not proactively maintain coherence among the DCaches of different cores, nor does it proactively handle stale copies or Dirty copies between the Scalar DCache and other GM access paths. Therefore, when the same GM address or the same Cache Line is accessed successively by different access paths, developers need to use DCCI, DSB, pipeline synchronization, and other methods to maintain cache coherence at the software level.

Scalar writes to GM can be divided into two categories:
- cache: Normal Scalar writes modify GM through the DCache, for example, directly writing to a `__gm__` address.
- no cache: Writes are issued directly to GM without going through the DCache, for example, `WriteGmBypassDCache`.

According to the access order, common scenarios that require software maintenance of coherence can be divided into three categories: RAW, WAW, and WAR.

##### RAW: Read After Write

The RAW (Read After Write) scenario requires that a subsequent read operation obtains the new value produced by a preceding write operation. When maintaining cache coherence in the RAW scenario, both the write end and the read end must be handled:

- If the write end writes to GM through the Scalar DCache, only a Dirty copy may be formed, and the data in GM has not been updated yet. Therefore, the write-end DCCI and DSB usually need to be executed before the consumer reads.
- If the read end reads through a cached path that may hold an old copy, the read-end DCCI needs to be executed according to the table before the actual read.
- If the write end and the read end are on different pipelines, the corresponding pipeline synchronization also needs to be inserted to ensure that the read operation does not execute before the post-write processing.

The following table lists the complete software maintenance operation sequence for Scalar writing to GM and then a consumer reading GM within the same AI Core. It is assumed that the producer and the consumer access the same address, and each cell is the complete operation sequence from post-write processing to pre-read processing.

In the table, cache indicates an access that goes through the cache, no cache indicates an access that does not go through the cache, `VF_SIMT` indicates a SIMT thread function that goes through the cache, MTE2 indicates a read operation through MTE2, MTE2 (cache) refers to [multidimensional data movement NDDMA](../../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md), and MTE2.DCI refers to the [invalidation operation](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md) of the NDDMA Cache or [asc_ndim_copy_dci](../../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_dci.md).

| Subsequent read method (column, occurs later)<br>╲<br>Preceding write method (row, occurs earlier) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE2 (cache) | MTE2 (no cache) |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar (cache) | Write Scalar.STORE first<br>Read Scalar.LOAD later | Write Scalar.STORE first<br>Write Scalar.DCCI first<br>Write Scalar.DSB first<br>Read Scalar.LOAD later | Write Scalar.STORE first<br>Write Scalar.DCCI first<br>Write Scalar.DSB first<br>sync S→V/VF<br>Read SIMT.DCCI later<br>Read VF_SIMT.LOAD later | Write Scalar.STORE first<br>Write Scalar.DCCI first<br>Write Scalar.DSB first<br>sync S→V/VF<br>Read VF_SIMT.LOAD later | Write Scalar.STORE first<br>Write Scalar.DCCI first<br>Write Scalar.DSB first<br>sync S→MTE2<br>Read MTE2.DCI later<br>Read MTE2.LOAD later | Write Scalar.STORE first<br>Write Scalar.DCCI first<br>Write Scalar.DSB first<br>sync S→MTE2<br>Read MTE2.LOAD later |
| Scalar (no cache) | Write Scalar.STORE first<br>Write Scalar.DSB first<br>Read Scalar.DCCI later<br>Read Scalar.LOAD later | Write Scalar.STORE first<br>Write Scalar.DSB first<br>Read Scalar.LOAD later | Write Scalar.STORE first<br>Write Scalar.DSB first<br>sync S→V/VF<br>Read SIMT.DCCI later<br>Read VF_SIMT.LOAD later | Write Scalar.STORE first<br>Write Scalar.DSB first<br>sync S→V/VF<br>Read VF_SIMT.LOAD later | Write Scalar.STORE first<br>Write Scalar.DSB first<br>sync S→MTE2<br>Read MTE2.DCI later<br>Read MTE2.LOAD later | Write Scalar.STORE first<br>Write Scalar.DSB first<br>sync S→MTE2<br>Read MTE2.LOAD later |

For example, when the preceding write method is `Scalar (cache)` and the subsequent read method is `VF_SIMT (cache)`, the corresponding cell requires the following to be executed:

```text
Scalar.STORE -> Scalar.DCCI -> Scalar.DSB -> sync S→V/VF -> SIMT.DCCI -> VF_SIMT.LOAD
```

The meaning is: first complete the write through `Scalar.STORE`; because this write may remain in the Scalar DCache, execute `Scalar.DCCI -> Scalar.DSB` to write the Dirty data back to GM and wait for the writeback to complete; then use `sync S→V/VF` to ensure that the VF_SIMT read occurs after the Scalar-side processing; finally, execute `SIMT.DCCI` before the VF_SIMT read to prevent the SIMT DCache from hitting an old copy, and then execute `VF_SIMT.LOAD` to read the new value.

If the preceding write is a `Scalar (no cache)` write, the write end does not leave a Dirty copy, but the write still needs to wait for completion through `Scalar.DSB`; if the subsequent read end uses a cached path, the read-end DCCI must still be executed according to the table to handle the old copy.

##### WAW: Write After Write

In a WAW (Write After Write) scenario, the data ultimately written to GM must be that of the later write operation. Because of DCache writeback, the later write in program order is not necessarily the last write in memory order. In the following example, `A` and `B` are located in the same 64B DCache Cache Line. The earlier write modifies `A` through the Scalar DCache, and the later write modifies `B` through MTE3. Although the MTE3 write occurs later, if the Dirty Cache Line in the Scalar DCache is written back to GM last, it may restore `B` to the old value in the Scalar cached copy:

```text
Initial state:
GM Cache Line = [A=0, B=0, ...]
A and B are located in the same 64B DCache Cache Line.

Time    Scalar pipeline                          MTE3 pipeline
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // A Dirty Cache Line is formed in the Scalar DCache:
          // [A=1, B=0, ...]
          // The GM Cache Line may still be [A=0, B=0, ...]

T1                                                MTE3.STORE B = 2
                                                  // MTE3 writes GM directly:
                                                  // GM Cache Line = [A=0, B=2, ...]

T2        Scalar.DCCI A
T3        Scalar.DSB
          // The Scalar Dirty Cache Line is written back to GM in 64B:
          // GM Cache Line = [A=1, B=0, ...]
          // The later write to B by MTE3 is overwritten by the earlier Scalar Dirty copy
```

Therefore, the key to maintaining the WAW scenario is to prevent the preceding Scalar Dirty copy from being written back to GM after the later write. The software should first process the Dirty copy left by the previous write or the preceding GM write wait according to the table, and then execute the later write. Whether the later write needs to further wait for completion or release is determined by the access path of the later write. The example above should be changed to:

```text
Initial state:
GM Cache Line = [A=0, B=0, ...]
A and B are located in the same 64B DCache Cache Line.

Time point    Scalar pipeline                          MTE3 pipeline
======    ====================================    ====================================
T0        Scalar.STORE A = 1
          // Form a Dirty Cache Line in the Scalar DCache:
          // [A=1, B=0, ...]

T1        Scalar.DCCI A
T2        Scalar.DSB
          // Complete the writeback of the preceding Scalar Dirty Cache Line:
          // GM Cache Line = [A=1, B=0, ...]

T3        sync S→MTE3
          // Establish the execution order from the Scalar pipeline to the MTE3 pipeline

T4                                                MTE3.STORE B = 2
                                                  // GM Cache Line = [A=1, B=2, ...]
```

The following table lists the operation sequence from the start of the preceding write, through the necessary preceding write processing and pipeline ordering, to the initiation of the subsequent write. When maintaining cache coherence in the WAW scenario, first find the previous write path by row, then find the later write path by column, and execute the operations in cell order. The `later write xxx.STORE` in the table only indicates that the later write operation has been initiated. If the later write needs to be immediately visible to GM or other access paths, the necessary DCCI operations still need to be performed according to the corresponding write path after the later write completes.

| Subsequent write mode (column, occurs later)<br>╲<br>Preceding write mode (row, occurs earlier) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar<br>(cache) | Preceding write Scalar.STORE<br>Later write Scalar.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DCCI<br>Preceding write Scalar.DSB<br>Later write Scalar.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DCCI<br>Preceding write Scalar.DSB<br>sync S→V/VF<br>Later write VF_SIMT.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DCCI<br>Preceding write Scalar.DSB<br>sync S→V/VF<br>Later write VF_SIMT.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DCCI<br>Preceding write Scalar.DSB<br>sync S→MTE3<br>Later write MTE3.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DCCI<br>Preceding write Scalar.DSB<br>sync S→FixPipe<br>Later write FixPipe.STORE |
| Scalar<br>(no cache) | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>Later write Scalar.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>Later write Scalar.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>sync S→V/VF<br>Later write VF_SIMT.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>sync S→V/VF<br>Later write VF_SIMT.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>sync S→MTE3<br>Later write MTE3.STORE | Preceding write Scalar.STORE<br>Preceding write Scalar.DSB<br>sync S→FixPipe<br>Later write FixPipe.STORE |

If the user cannot guarantee the ordering among multiple writers, multiple cores should be prevented from writing the same address or the same Cache Line through the DCache.

##### WAR: Write After Read

The WAR (Write After Read) scenario requires that the preceding read operation reads the old value that existed before the subsequent write operation occurs. The key to such scenarios is not the write result, but ensuring that the read operation has completed before allowing the subsequent write to be issued. Otherwise, the subsequent write may prematurely change the value in GM, causing the preceding read to observe a new value that it should not have seen.

The following table lists the operation sequence from the start of the preceding read, through the necessary waits and pipeline ordering, to the issuance of the subsequent write, when the preceding read method is `Scalar (cache)` or `Scalar (no cache)`. In cross-core scenarios, the intra-core `sync` in the table must be replaced with inter-core notification.

| Subsequent write method (column, occurs later)<br>╲<br>Preceding read method (row, occurs first) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| Scalar (cache) | Preceding read Scalar.LOAD<br>Subsequent write Scalar.STORE | Preceding read Scalar.LOAD<br>Preceding read Scalar.DSB<br>Subsequent write Scalar.STORE | Preceding read Scalar.LOAD<br>sync S→V/VF<br>Subsequent write VF_SIMT.STORE | Preceding read Scalar.LOAD<br>sync S→V/VF<br>Subsequent write VF_SIMT.STORE | Preceding read Scalar.LOAD<br>sync S→MTE3<br>Subsequent write MTE3.STORE | Preceding read Scalar.LOAD<br>sync S→FixPipe<br>Subsequent write FixPipe.STORE |
| Scalar (no cache) | Preceding read Scalar.LOAD<br>Preceding read Scalar.DSB<br>Subsequent write Scalar.STORE | Preceding read Scalar.LOAD<br>Preceding read Scalar.DSB<br>Subsequent write Scalar.STORE | Preceding read Scalar.LOAD<br>sync S→V/VF<br>Subsequent write VF_SIMT.STORE | Preceding read Scalar.LOAD<br>sync S→V/VF<br>Subsequent write VF_SIMT.STORE | Preceding read Scalar.LOAD<br>sync S→MTE3<br>Subsequent write MTE3.STORE | Preceding read Scalar.LOAD<br>sync S→FixPipe<br>Subsequent write FixPipe.STORE |

The goal of maintaining cache coherence in the WAR scenario is to read the old value before the write. The maintenance focus is to ensure that this read has completed. The `DSB` in the table is only responsible for waiting for the preceding GM access to complete, and cannot replace inter-pipeline or inter-core synchronization. `sync S→V/VF`, `sync S→MTE3`, and `sync S→FixPipe` are only used to establish pipeline ordering within the same AI Core. If the preceding access and the subsequent access are located on different AI Cores, the intra-core pipeline synchronization in the table cannot take effect across cores, and cross-core communication must be organized in the following order:

```text
Preceding access core: complete the Cache processing on the preceding access side and the necessary DSB -> send inter-core notification
Subsequent access core: wait for inter-core notification -> Cache copy processing -> access data
```

### Cache Coherence Between ICache and GM

ICache is used to cache the instructions most recently or frequently used by the Scalar unit. When the Scalar unit reads instructions from GM, it first reads from ICache. If ICache misses, it reads from L2 Cache. If L2 Cache also misses, it loads the instructions from GM and updates L2 Cache and ICache. For details, see [System Cache Overview](../../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md).

From the perspective of cache coherence, ICache caches instructions rather than normal data, and it is a read-only cache. Therefore, it generally does not involve the problem of inconsistent cache copies on the normal data path.

<!-- npu="950" id4 -->
### Cache Coherence Between SIMT DCache and GM

#### SIMT DCache Access Path

[NPU architecture version 3510](../../language_extension/simd_builtin_keywords.md) adds SIMT-related hardware units. SIMT memory access involves the SIMT DCache: the SIMT DCache reuses a portion of the UB space to cache data when SIMT threads access GM. The SIMT DCache space is configurable from 32 KB to 128 KB. The actual size is determined by subtracting static memory, dynamic memory, and reserved space from the total UB size. If the remaining space exceeds 128 KB, the SIMT DCache size is fixed at 128 KB. SIMT also provides interfaces such as [`asc_ldcg`](../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md) and [`asc_stcg`](../../../../api/SIMT-API/memory_access_functions/asc_stcg.md) to express the access intent of not occupying or bypassing the SIMT DCache. Therefore, this section analyzes the cache path and the no-cache path separately.

The underlying implementation of SIMT reading GM: first read from the SIMT DCache; on a miss, access the L2 Cache or GM, and cache the data into both the L2 Cache and the SIMT DCache. Data coherence between the L2 Cache and GM is guaranteed by hardware, but coherence between the SIMT DCache and GM is not guaranteed. Therefore, if the same GM address has already been updated by other cores, the Scalar path, or the MTE path, while the current SIMT DCache still holds an old copy, subsequent SIMT reads may continue to hit the old value.

The SIMT DCache adopts a write-through policy:

- When SIMT writes GM, the write enters the SIMT DCache and is simultaneously written out to GM, without forming a Dirty copy that needs to be cleaned back to GM later.
- A no-cache write does not go through the SIMT DCache, nor does it cache the written data in the SIMT DCache. When a no-cache write accesses GM, it checks whether the data at that address is already cached in the SIMT DCache. On a hit, the hardware performs DCCI by itself.

To determine whether cache handling is required after a SIMT write to GM, follow two steps:

- **First, check whether the write side needs to wait for the GM write to complete**: The SIMT DCache is a write-through cache, so the write side does not need to perform DCCI to write the Dirty copy back to GM.
- **Then, check whether the read side may hit an old copy**: If the consumer reads through another cache path such as Scalar or MTE2, the read-side path may already hold an old copy, and the corresponding DCCI must be performed before reading according to the table.

If threads or cores also communicate through flag bits on GM, memory fences such as [`asc_threadfence`](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) must be used separately to handle the write ordering relationship. Memory fences belong to memory ordering control; they do not automatically write back the producer-side cache, nor do they automatically invalidate the consumer-side cache.

#### Software Maintenance of VF_SIMT Access to GM Cache Coherence

This section describes, from the software maintenance perspective, how to maintain SIMT DCache coherence when VF_SIMT accesses GM. The hardware only maintains cache coherence of the SIMT pipeline within a single core, and does not maintain coherence between SIMT and other pipelines. Because the SIMT DCache uses a write-through policy, VF_SIMT writes to GM do not create Dirty copies that need to be subsequently cleaned back to GM. However, whether the write operation has completed, whether the read side may hit an old copy, and whether ordering has been established between different pipelines still need to be handled by software according to the access dependencies.

The meanings of the two types of VF_SIMT access paths in the table are as follows:

- **VF_SIMT (cache)**: Writes go through the SIMT DCache write-through path. The data is cached to the SIMT DCache and L2 Cache, and is also written out to GM.
- **VF_SIMT (no cache)**: Writes to GM do not go through the SIMT DCache, and the written data is not cached in the SIMT DCache.

Because the SIMT DCache is a write-through cache and the no cache path does not produce Dirty copies either, the software coherence maintenance actions required by the two in RAW, WAW, and WAR scenarios are basically the same. In the following tables, `VF_SIMT (cache)` and `VF_SIMT (no cache)` are merged into `VF_SIMT (cache/no cache)`.

The following analyzes the three types of dependencies: RAW, WAW, and WAR. The tables in this section are also read "row first, then column": a row represents the access that occurs first, and a column represents the access that occurs later; the operations in a cell are executed from top to bottom. In the tables, `xxx.STORE` represents the write operation of the corresponding path, and `xxx.LOAD` represents the read operation of the corresponding path. `SIMT.DCCI` is used to invalidate old copies in the SIMT DCache, `sync X→Y` is used to establish the execution order from pipeline X to pipeline Y within the same AI Core, and `Scalar.DCCI` and `MTE2.DCI` indicate that the read side processes possible old Cache copies on this path before reading. `DSB` and `sync` do not change the state of Cache copies and cannot replace DCCI.

##### RAW: Read After Write

In the RAW scenario, the subsequent read operation must read the new value produced by the preceding VF_SIMT write operation. Because the SIMT DCache is a write-through cache, the write end does not need to execute DCCI to write back Dirty data. If the subsequent read end is a cache-bearing path such as Scalar or MTE2 that may hold an old copy, the corresponding DCCI must also be executed before the read to avoid hitting the old copy.

The following table describes which Cache processing operations and necessary wait/pipeline ordering operations the producer and consumer must execute in sequence after the intra-core VF_SIMT writes GM as the producer and before the consumer actually reads.

| Subsequent read mode (column, occurs later)<br>╲<br>Preceding write mode (row, occurs first) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE2 (cache) | MTE2 (no cache) |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>(cache/no cache) | Write VF_SIMT.STORE first<br>sync V/VF→S<br>Read Scalar.DCCI later<br>Read Scalar.LOAD later | Write VF_SIMT.STORE first<br>sync V/VF→S<br>Read Scalar.LOAD later | Write VF_SIMT.STORE first<br>sync SIMT->SIMT (no synchronization required within the same thread)<br>Read VF_SIMT.LOAD later | Write VF_SIMT.STORE first<br>sync SIMT->SIMT (no synchronization required within the same thread)<br>Read VF_SIMT.LOAD later | Write VF_SIMT.STORE first<br>sync V/VF→MTE2<br>Read MTE2.DCI later<br>Read MTE2.LOAD later | Write VF_SIMT.STORE first<br>sync V/VF→MTE2<br>Read MTE2.LOAD later |

For example, when the preceding write mode is `VF_SIMT (cache/no cache)` and the subsequent read mode is normal `Scalar (cache)`, the following operations must be executed:

```text
VF_SIMT.STORE -> sync V/VF→S -> Scalar.DCCI -> Scalar.LOAD
```

The meaning is: the SIMT DCache is a write-through cache, so when SIMT writes GM, the new value is written out to GM. Before the consumer reads, `sync V/VF→S` ensures that the Scalar read within the same AI Core occurs after the VF_SIMT write, and `Scalar.DCCI` is executed to avoid an existing old copy in the Scalar DCache. Only after that can the Scalar read obtain the new value from the correct location.

Other cells are read in the same way: first complete the VF_SIMT write described in the row, and then supplement the wait, pipeline synchronization, and read-end Cache invalidation operations according to the subsequent read path in the column. Whether the read-end DCCI is required depends on whether the subsequent read path may hit an old Cache copy.

Here, `sync SIMT->SIMT (no synchronization required within the same thread)` in the table means that if the same address is operated across threads, phase synchronization must be established through [asc_syncthreads](../../../../api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md). If the VF_SIMT write and read of the same address occur within a single SIMT thread, no synchronization is required.

##### WAW: Write After Write

In a WAW scenario, the data finally written to GM must be that of the later write operation. Because the SIMT DCache is a write-through cache, the earlier VF_SIMT write does not leave a Dirty copy pending writeback to GM. The focus of software maintenance is to guarantee the execution order of the two writes according to the table, preventing the earlier write from reaching GM late and affecting the result of the later write.

The following table lists the operation sequence from the start of the preceding write, through the necessary preceding-write wait and pipeline ordering, to the initiation of the subsequent write. When using the table, first select the earlier VF_SIMT write path by row, then select the later write path by column, and execute in cell order. The `later-write xxx.STORE` in the table only indicates that the later write operation has been initiated; it does not indicate that the later write has completed, nor that the result of the later write is already visible to GM or other access paths. The wait and release after the later write completes must be handled separately according to the later write path.

| Subsequent write method (column, occurs later)<br>╲<br>Preceding write method (row, occurs earlier) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>(cache/no cache) | Earlier write VF_SIMT.STORE<br>sync V/VF→S<br>Later write Scalar.STORE | Earlier write VF_SIMT.STORE<br>sync V/VF→S<br>Later write Scalar.STORE | Earlier write VF_SIMT.STORE<br>sync SIMT->SIMT (no synchronization needed within the same thread)<br>Later write VF_SIMT.STORE | Earlier write VF_SIMT.STORE<br>sync SIMT->SIMT (no synchronization needed within the same thread)<br>Later write VF_SIMT.STORE | Earlier write VF_SIMT.STORE<br>sync V/VF→MTE3<br>Later write MTE3.STORE | Earlier write VF_SIMT.STORE<br>sync V/VF→FixPipe<br>Later write FixPipe.STORE |

If the later write needs to be immediately visible to GM or other access paths, the necessary wait or release operations must still be performed after the later write completes, according to the corresponding write path. For example, when the later write is Scalar (cache), you need to execute `later-write Scalar.DCCI -> later-write Scalar.DSB`; when the later write is VF_SIMT, MTE3, or FixPipe, you need to wait for the corresponding write-to-GM operation to complete.

##### WAR: Write After Read

The WAR scenario requires the preceding read operation to obtain the old value that existed before the subsequent write operation. Such scenarios do not require the new value to be published to the read end as soon as possible. Instead, you must first confirm that the read operation on the VF_SIMT side has completed, and then allow the subsequent write end to perform the write-to-GM operation or send the write completion notification.

The following table lists the operation sequence from the start of the preceding read, through the necessary waits and pipeline ordering, to the initiation of the subsequent write, when the preceding read mode is `VF_SIMT (cache/no cache)`. To use the table, first select the preceding VF_SIMT read path by row, and then select the subsequent write path by column. In a cell, `pre-read VF_SIMT.LOAD` indicates the old-value read point, and `post-write xxx.STORE` indicates the new-value write point. In cross-core scenarios, replace the intra-core `sync` in the table with inter-core notification.

| Subsequent write mode (column, occurs later)<br>╲<br>Preceding read mode (row, occurs earlier) | Scalar (cache) | Scalar (no cache) | VF_SIMT (cache) | VF_SIMT (no cache) | MTE3 | FixPipe |
| --- | --- | --- | --- | --- | --- | --- |
| VF_SIMT<br>(cache/no cache) | pre-read VF_SIMT.LOAD<br>sync V/VF→S<br>post-write Scalar.STORE | pre-read VF_SIMT.LOAD<br>sync V/VF→S<br>post-write Scalar.STORE | pre-read VF_SIMT.LOAD<br>sync SIMT->SIMT (no synchronization required within the same thread)<br>post-write VF_SIMT.STORE | pre-read VF_SIMT.LOAD<br>sync SIMT->SIMT (no synchronization required within the same thread)<br>post-write VF_SIMT.STORE | pre-read VF_SIMT.LOAD<br>sync V/VF→MTE3<br>post-write MTE3.STORE | pre-read VF_SIMT.LOAD<br>sync V/VF→FixPipe<br>post-write FixPipe.STORE |

The WAR table does not require the read end to invalidate the Cache copy, because the goal is to read the old value before the write. The maintenance focus is to wait for the preceding read to complete and to establish ordering to the subsequent write path. If the preceding access and the subsequent access are on different AI Cores, the `sync V/VF→S`, `sync V/VF→MTE3`, and `sync V/VF→FixPipe` in the table cannot take effect across cores. In this case, send an inter-core notification after the read on the core where the preceding access resides completes. After the core where the subsequent access resides receives the notification, it then handles the Cache copies that may exist on the read end or write end of this core.
<!-- end id4 -->

### Cache Coherence Between UB and DCache

The coherence issue between UB and DCache mainly occurs when Scalar enables DCache mode to read UB: when Scalar accesses UB, `CTRL[49]` controls whether Scalar reads UB through DCache. When `CTRL[49] = 1'b0`, Scalar reads directly from UB. When `CTRL[49] = 1'b1`, Scalar reads UB data through DCache. For details, see [asc_dcci](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md) and [asc_set_ctrl()](../../../../api/SIMD-API/c_api/spr/asc_set_ctrl.md).

#### Cache Coherence Brought by Scalar Accessing UB in DCache Mode

By default, Scalar reads UB directly. However, if the user explicitly sets `CTRL[49] = 1'b1`, Scalar reads UB in DCache mode, in which case the cache coherence issue brought by Scalar accessing UB in DCache mode must be handled.

After Scalar reads UB, the data enters DCache. When Scalar subsequently reads the same UB address range again, it may continue hitting the DCache copy instead of reading UB directly. The following uses a UB address `A` within the same core to illustrate the old copy issue.

```text
Initial state:
UB[A] = 0
A is not cached in Scalar DCache
CTRL[49] = 1'b1    // Scalar reads from DCache when reading UB

Time point    Vector pipeline                          Scalar pipeline
======    ====================================    ====================================
T0                                                Scalar.LOAD UB[A] -> 0
                                                  // Scalar DCache caches the Cache Line where A is located

T1        Vector.STORE UB[A] = 1
          // UB[A] = 1

T2        sync V→S
          // Ensure that the subsequent Scalar read occurs after Vector writes UB

T3                                                Scalar.LOAD UB[A] -> 0
                                                  // Scalar may continue hitting the old DCache copy
                                                  // Even if UB[A] is already 1, 0 may still be read
```

This scenario involves two independent problems: first, whether the Vector write to UB has completed, and second, whether an old UB copy has already been cached in the Scalar DCache. Even if the Vector write to UB has completed, the Scalar may still continue hitting the old DCache copy. Therefore, before the consumer reads UB through the Scalar DCache mode, it needs to handle both the pipeline order and the old DCache copy:

```text
Initial state:
UB[A] = 0
A is not cached in the Scalar DCache
CTRL[49] = 1'b1    // When the Scalar reads UB, it reads from the DCache

Time    Vector pipeline                              Scalar pipeline
======    ====================================    ====================================
T0                                                Scalar.LOAD UB[A] -> 0
                                                  // The Scalar DCache caches the cache line where A resides

T1        Vector.STORE UB[A] = 1
          // UB[A] = 1

T2        sync V→S
          // Ensure that the subsequent Scalar read occurs after the Vector writes UB

T3                                                Scalar.DCCI
                                                  // Invalidate the old UB[A] copy cached in the Scalar DCache

T4                                                Scalar.DSB
                                                  // Wait for the DCCI-related operations to complete

T5                                                Scalar.LOAD UB[A] -> 1
                                                  // Scalar fetches the new value from UB again.
```

Here, `sync V->S` is used to ensure that the Scalar read within the same AI Core occurs after the Vector write to UB; `Scalar.DCCI` is used to handle the coherence between DCache and UB so that subsequent Scalar reads no longer use the old DCache copy; `Scalar.DSB` is used to wait for the completion of DCCI-related operations. If the producer is MTE2 moving GM data into UB, it is also necessary to first establish the `MTE2 -> S` pipeline order, and then handle the old UB copy in the Scalar DCache.

#### Cache Handling Principles

The C API provides DCCI interfaces for the UB address space, such as [`asc_ub_dcci_single`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md) and [`asc_dcci_entire_ub`](../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md). In actual use, select the supported UB DCCI interface based on the current product and CANN version. After the DCCI is issued, use DSB to wait for the related operations to complete before performing subsequent Scalar reads.

In actual programming, first determine where the consumer reads from, and then select the corresponding coherence target:

- If the subsequent consumer directly reads UB in Scalar DCache mode, the focus is on the coherence between DCache and UB. Use DCCI to handle the old copy, and establish synchronization from the producer pipeline to the Scalar pipeline before reading.
- If DCache mode is not enabled when Scalar reads UB, that is, `CTRL[49] = 1'b0`, Scalar reads directly from UB. In this case, you usually only need to handle the execution order between the Scalar pipeline and the Vector/MTE pipeline, and do not need to use DCCI to handle the old DCache copy.


## Atomic Operations and Coherence

Atomic operations are used when multiple execution units, multiple threads, or multiple cores concurrently update the same address, treating a single read-modify-write process as an event that cannot be interrupted by other updates. Atomic operations can prevent lost updates on the same address, but they are not equivalent to cache coherence handling, nor are they equivalent to a synchronization barrier. To determine cache coherence under atomic operations, two questions must be considered simultaneously: whether the atomic operation actually updates GM or UB, and whether the same address is also accessed by other paths such as the normal DCache<!-- npu="950" id5 -->, SIMT DCache<!-- end id5 -->, or MTE.

The following classifies atomic events by their initiation path:<!-- npu="950" id6 -->Scalar atomic,<!-- end id6 -->MTE3 in-band atomic, FixPipe in-band atomic<!-- npu="950" id7 -->, and SIMT atomic<!-- end id7 -->. This section only describes the Cache behavior that must be considered when these atomic events are incorporated into coherence analysis. For interface details, see the corresponding API documentation.

<!-- npu="950" id8 -->
### Scalar Atomic Operations and DCache Coherence

[NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) supports Scalar atomic operations. When Scalar atomic operations are mixed with normal Scalar read/write of GM, the coherence between DCache and GM must be analyzed. For details, see [DCache Cache Coherence Caused by Atomic Access](#dcache-cache-coherence-caused-by-atomic-access).
<!-- end id8 -->

### MTE3 Inline Atomic Operation and DCache Coherence

An inline atomic operation is not a Scalar event that reads a specific address independently; instead, it changes the data movement behavior whose subsequent destination address is GM. MTE3 is responsible for moving data within the core, such as UB, out to GM, so `UB -> GM` is a common access path for inline atomics. For API details, see the C APIs [asc_set_atomic_add](../../../../api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md) and [asc_disable_dma_atomic](../../../../api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md).

From a coherence perspective, the target of an MTE3 inline atomic is GM, and it does not automatically handle the existing copy in the Scalar DCache. Therefore, the following relationships need to be considered:

- Before enabling the inline atomic, if the GM target address was previously written by a normal Scalar write, ensure that the Dirty data in the Scalar DCache has been written back to GM first. Otherwise, the "existing GM content" read by the atomic operation may not be the initial value expected by the program.

- After the inline atomic completes, if the same GM address is subsequently read by a normal Scalar read, handle the old copy in the read-end DCache and wait for MTE3 to finish writing to GM.


### FixPipe In-Band Atomic Operation and DCache Coherence

FixPipe is commonly used in the `L0C Buffer -> GM` or related cube move-out path. For coherence analysis, a FixPipe in-band atomic operation can be regarded as another type of "in-band atomic event that writes to GM": the data source differs from MTE3, but the final update is still to the GM target address.

Therefore, the cache coherence behavior of a FixPipe in-band atomic operation is similar to that of an MTE3 in-band atomic operation: it does not automatically clean the old Scalar DCache copy, nor does it automatically merge the Dirty data previously residing in the DCache into GM.

- If the initial value of the GM target address comes from a normal Scalar write to GM, first write back the Dirty data in the DCache to GM according to [DCache Cache Coherence](#cache-coherence-between-dcache-and-gm), and then execute the FixPipe in-band atomic operation.
- If, after the FixPipe in-band atomic operation, the consumer reads GM through the normal Scalar path, wait for the FixPipe write to GM to complete, and handle the old DCache copy on the read side.
- If multiple pipelines or multiple cores perform in-band atomic operations on the same GM address, the atomic operation only guarantees that each GM read-modify-write is not interrupted; the launch order among different pipelines and different cores must still be guaranteed by synchronization or algorithm protocols.

<!-- npu="950" id9 -->
### SIMT Atomic Operations and Cache Coherence

[NPU Architecture Version 3510](../../language_extension/simd_builtin_keywords.md) supports SIMT atomic operations. SIMT atomic operations may act on UB or GM. For an interface example, see [asc_atomic_add](../../../../api/SIMT-API/atomic_operations/asc_atomic_add.md). It guarantees the atomicity of this read-modify-write operation on the same address, but does not guarantee the execution order among multiple threads.

When analyzing cache coherence, first distinguish the target address:

- When the target is UB, focus on the visibility and execution order of shared UB data among threads within the same Thread Block. If the same UB data is also read by Scalar in DCache mode, handle it together with [Coherence Between DCache and UB](#cache-coherence-between-ub-and-dcache).
- When the target is GM, analyze SIMT atomic updates to GM, SIMT normal read/write of GM, Scalar normal read/write of GM, and MTE transfers of GM in the same visibility graph. Atomic operations themselves cannot replace the handling of stale copies in SIMT DCache or Scalar DCache. If the same GM address is subsequently read through the default SIMT read or Scalar normal read, you still need to handle the possible stale Cache copies on the read end according to [SIMT Cache Coherence](#cache-coherence-between-simt-dcache-and-gm) or [DCache Cache Coherence](#cache-coherence-between-dcache-and-gm).
- If different Thread Blocks or different AI Cores communicate through GM flags combined with SIMT atomic operations, you still need to establish a complete synchronization relationship by combining memory fences, atomic operations, polling protocols, and necessary Cache handling, rather than relying solely on atomic operations themselves.
<!-- end id9 -->

## Prefetch

Prefetch is used to load data or instructions that may be accessed later into the Cache in advance, with the goal of reducing access latency. For details, see [System Cache Overview](../../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md). From a coherence perspective, prefetch changes the timing at which data or instructions enter the Cache, which may cause coherence issues.

#### L2 Cache Prefetch

There is no coherence issue between the L2 Cache and GM, so L2 Cache prefetch does not cause data inconsistency among multiple cores. However, this does not mean that the data obtained by L2 Cache prefetch is the expected data: if the new value written by the producer is still in the DCache, the MTE3 write to GM has not completed, or the consumer's read order is earlier than the completion of the producer's write, the value loaded into the L2 Cache in advance may still be the old value visible to GM. In this case, refer to the corresponding sections to handle the producer's writeback, wait for access completion, and invalidate the old copy on the read end.

#### DCache Prefetch

[asc_datacache_preload](../../../../api/SIMD-API/c_api/cache_ctrl/asc_datacache_preload.md) is used to preload data from a specified GM address into the DCache. Each call can preload only one Cache Line of data. Its purpose is to make subsequent Scalar reads more likely to hit the DCache, rather than to establish a new synchronization or coherence relationship.

The DCache is an independent cache within each core, so DCache prefetch forms a copy of GM data in the current core's DCache in advance. If, after prefetching, the same GM address is updated by another core, the MTE, an atomic operation, or the SIMT path, subsequent normal Scalar reads on the current core may continue to hit the old copy formed by the prefetch. When the latest GM data needs to be read, you must still execute DCCI to invalidate the old copy as described in [DCache Cache Coherence](#cache-coherence-between-dcache-and-gm), or use a read API that bypasses the DCache.

#### ICache Prefetch

[asc_icache_preload](../../../../api/SIMD-API/c_api/cache_ctrl/asc_icache_preload.md) is used to preload instructions from the GM address where the instruction resides into the ICache. ICache prefetch mainly affects instruction fetch performance. It does not participate in the producer-consumer visibility determination of normal data, and it has no coherence issue.

## Summary

The core of the cache coherence problem is that the same data may exist simultaneously in different locations such as GM, DCache, <!-- npu="950" id10 -->SIMT DCache, <!-- end id10 -->UB. Software needs to explicitly handle Cache copies and execution order based on the access path and RAW, WAW, and WAR dependencies.

In actual programming, focus on the following rules:

- When the write end may have a Dirty copy, write it back through DCCI and wait for completion with DSB.
- When the read end may have an old Cache copy, invalidate the old copy through DCCI before reading.
- DCCI handles the Cache copy state, DSB waits for preceding memory accesses to complete, and pipeline synchronization or inter-core notification establishes the execution order. The three cannot replace each other.
