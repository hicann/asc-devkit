# System Cache Overview<a name="ZH-CN_TOPIC_0000002594407823"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T12:23:33.164Z -->

## Cache type<a name="zh-cn_topic_0000002583420201_section112105011415"></a>

The main purpose of Cache is to provide a high-speed buffer between the transfer unit or Scalar unit and external storage, thereby reducing data access latency and improving bandwidth utilization. Typically, frequently accessed data is written to Cache, and the transfer unit or Scalar unit prefers to read data from Cache during execution. On cache miss, data is loaded from external storage and updated to Cache.

[Table 1](#table1) shows the functions of different Cache types. The Cache types supported vary by product, as described below:
<!-- npu="950" id1 -->
Taking Ascend 950PR/Ascend 950DT as an example, Figure 1 shows the positional relationships of the five Cache types supported in the AI Core (L2 Cache, DCache, ICache, SIMT DCache, and NDDMA Cache) within the hardware architecture.

**Figure 1**  Positional relationship diagram of the five Cache types in the AI Core  
![](../../../../api/figures/atlas_950_cache_architecture_diagram.png "atlas_950_cache_architecture_diagram")
<!-- end id1 -->
<!-- npu="A3,910b" id2 -->
Taking [NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) as an example, Figure 2 shows the positional relationships of the three Cache types supported in the AI Core (L2 Cache, DCache, and ICache) within the hardware architecture.

**Figure 2**  Positional relationship diagram of the three Cache types in the AI Core<a name="zh-cn_topic_0000002583420201_fig496311278412"></a>  
![](../../../../api/figures/atlas_a2_a3_cache_architecture_diagram.png "atlas_a2_a3_cache_architecture_diagram")
<!-- end id2 -->

**Table 1**  Cache types and function descriptions <a name="table1"></a>

| Cache type | Description |
|------------|------|
| L2 Cache | L2 Cache serves as the second-level cache, specifically used for caching frequently accessed data and instructions to reduce reads and writes to GM.<br>&bull;When reading GM through the MTE2 unit, data is preferentially read from L2 Cache. On L2 Cache miss, data is loaded from GM and updated to L2 Cache.<br>&bull;When reading GM through the Scalar unit, data is preferentially read from DCache. On DCache miss, data is read from L2 Cache. When L2 Cache also misses, data is loaded from GM and updated to both L2 Cache and DCache.<br>&bull;When reading GM instructions through the Scalar unit, instructions are preferentially read from ICache. On ICache miss, instructions are read from L2 Cache. When L2 Cache also misses, instructions are loaded from GM and updated to both L2 Cache and ICache. |
| DCache | DCache (Data Cache) is used for caching data segments that the Scalar unit may repeatedly access in the near future.<br>When reading GM through the Scalar unit, data is preferentially read from DCache. On DCache miss, data is read from L2 Cache. When L2 Cache also misses, data is loaded from GM and updated to both L2 Cache and DCache. |
| ICache | ICache (Instruction Cache) is used for caching instructions recently or frequently used by the Scalar unit.<br>When reading GM instructions through the Scalar unit, instructions are preferentially read from ICache. On ICache miss, instructions are read from L2 Cache. When L2 Cache also misses, instructions are loaded from GM and updated to both L2 Cache and ICache. |
| SIMT DCache | SIMT access to GM must be relayed through SIMT DCache. The data flow goes from GM to SIMT DCache, and then from SIMT DCache to SIMT registers. SIMT DCache acts as an intermediate buffer layer, reducing the number of direct accesses to GM. |
| NDDMA Cache | NDDMA Cache is used for caching data that is recently or about to be transferred by the [multi-dimensional data transfer DataCopy](../../basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md) API. |

## Cache Operations and Usage Scenarios<a name="zh-cn_topic_0000002583420201_section054554323117"></a>

Table 2 describes the four basic cache operations and their core functions.

**Table 2**  Cache operations and function descriptions

| Operation name | Function description |
|---------|---------|
| Prefetch | The hardware automatically loads the data expected to be accessed into the cache in advance based on the access pattern, improving subsequent access speed. |
| Preload | The software proactively loads the specified data into the cache through explicit instructions to prepare for upcoming read and write operations. |
| Invalid | &bull;Marks the **Cache Line** (the smallest unit of cache operation) in the specified address range as "invalid", removing it from the cache.<br>&bull;Ensures that the next access to these memory addresses reloads data from **GM** instead of using possibly stale cached data.<br>&bull;Note: The Invalid operation does not check whether a cache line is "dirty" (dirty indicates that the data has been modified but not yet written back to **GM**). If dirty data exists, it is discarded directly. |
| Clean | Writes the modified data (dirty data) in the cache back to **GM** to avoid data loss. The Clean operation does not mark the cache line as "invalid"; the cache line remains valid. |

## Cache Write Policy and Cache Coherency Issues<a name="zh-cn_topic_0000002583420201_section053731716357"></a>

When data is written to GM with Cache enabled, the following two Cache write policies are available: Write-Back and Write-Through.

When the Write-Back policy is adopted for writing data to GM, the working principle is as follows:

- The developer expects to write data to GM (that is, expects the data to be written directly to GM), but the actual hardware behavior is as follows:
    - When data is modified, it is updated only in the Cache.
    - The data in the Cache is marked as "dirty".
    - Only when the Cache Line is replaced (or when the software explicitly performs a Clean operation) is the data written back to GM. When the Cache Line is replaced is determined by the hardware.

- When replacing Cache Line data:
    - When Cache space is insufficient and data needs to be replaced, check whether the data to be replaced is "dirty".
    - If the data is "dirty", write it back to GM first, and then replace the Cache Line out of the Cache.

When writing data to GM, if the write-through strategy is adopted, its working principle is as follows:

- The developer expects to write data to GM (that is, expects the data to be written directly to GM), while the actual hardware behavior is as follows:
    - When data is modified, the data is updated in both the cache and GM immediately.
    - The cache line does not need to be marked as "dirty".

- When replacing cache line data:

    Since every data write synchronously updates GM, the data in the cache does not have a "dirty" state, and there is no need to additionally write back to GM during replacement.

Under the write-back strategy, modified data is updated only in the local cache and is not immediately synchronized to GM. When other cores read the same address in GM, they will obtain stale data, resulting in inconsistent cache data among different cores.
Cache coherence is the core mechanism for ensuring data correctness in a multi-core system. In simple terms, when multiple cores each have their own independent cache, they may cache the same data simultaneously. If one core modifies the data in its cache, the cache copies of other cores become "stale". If they are not synchronized in time, data inconsistency among multiple cores will occur, which in turn leads to computation errors.

<!-- npu="950,A3,910b" id5 -->
The following describes whether each type of cache in different products needs to consider inter-core data inconsistency:
<!-- npu="950" id3 -->
- For Ascend 950PR/Ascend 950DT, the five types of cache supported by this product are as follows in terms of inter-core data consistency:
    - L2 Cache: A cache shared among multiple cores, so there is no need to consider the problem of inter-core data inconsistency.
    - DCache: A multi-core independent cache, so inter-core data consistency must be considered.
    - ICache: Read-only, so inter-core data inconsistency does not need to be considered.
    - NDDMA Cache: A multi-core independent cache, so inter-core data consistency must be considered.
    - SIMT DCache: In SIMT, data is written to GM immediately when written, so there is no consistency issue; when reading data from GM, it is a multi-core independent cache, so inter-core data consistency must be considered.
<!-- end id3 -->
<!-- npu="A3,910b" id4 -->
- For [NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md), the three supported cache types behave as follows in terms of inter-core data consistency:
    - L2 Cache: A cache shared among multiple cores, so inter-core data inconsistency does not need to be considered.
    - DCache: A multi-core independent cache, so inter-core data consistency must be considered.
    - ICache: Read-only, so inter-core data inconsistency does not need to be considered.
<!-- end id4 -->
<!-- end id5 -->
## Cache Control API Summary<a name="zh-cn_topic_0000002583420201_section489931219616"></a>

Table 3 summarizes the APIs related to cache control and their function descriptions by Cache type (L2 Cache, DCache, ICache, and NDDMA Cache).

**Table 3**  Cache Control API Summary

| Cache type | API name | Function description |
|-----------|---------|---------|
| L2 Cache | [SetL2CacheHint](../data_structures/GlobalTensor/SetL2CacheHint.md) | Data read from and written to GM is cached in L2 Cache by default (L2 Cache is enabled by default). **SetL2CacheHint** can set whether **GlobalTensor** enables L2 Cache. |
| DCache | [DataCachePreload](DataCachePreload.md) | The **DataCachePreload** API preloads data from the specific GM address where the source address resides into DCache. Each call can preload only one Cache Line of data. |
| DCache | [DataCacheCleanAndInvalid](DataCacheCleanAndInvalid.md) | When the Scalar unit accesses GM, use this API to refresh the Cache to ensure Cache consistency. The usage scenarios are as follows:<br>&bull;When reading data from GM that may have been modified by other cores externally, use the **DataCacheCleanAndInvalid** API to directly access GM and obtain the latest data.<br>&bull;When the user writes data to GM through the Scalar unit and wants it to be written out immediately, the **DataCacheCleanAndInvalid** API is also required. |
| DCache | [ReadGmByPassDCache](../scalar_compute/ReadGmByPassDCache_ISASI.md)/[WriteGmByPassDCache](../scalar_compute/WriteGmByPassDCache_ISASI.md) | **Read**/**Write** data to the GM address without going through DCache. Usage scenarios:<br>&bull;When data written by multiple cores falls within the same Cache Line, reads and writes through DCache are performed at a 64B granularity, which may cause random overwriting of multi-core data. Using this API to directly **read**/**write** GM by operand size without going through DCache can avoid this problem.<br>&bull;Using this API to directly **read**/**write** data to GM without going through DCache can avoid the inter-core data inconsistency problem caused by DCache caching. |
| ICache | [ICachePreLoad](ICachePreLoad_ISASI.md) | Developers can manually call the **ICachePreLoad** API to preload instructions from the GM address where the instructions reside into ICache. |
| ICache | [GetICachePreloadStatus](GetICachePreloadStatus_ISASI.md) | **GetICachePreloadStatus** is a debugging API. Call it after calling **ICachePreLoad** to obtain the PreLoad status of ICache. When the return value is 0, the ICache PreLoad is complete; when the return value is 1, the ICache PreLoad is not complete. |
| NDDMA Cache | NdDmaDci | Before using the [multi-dimensional data transfer DataCopy](../../basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md) API for data transfer, use the **NdDmaDci** API to refresh the NDDMA Cache to ensure it is in the latest state. |
