# Improving Hash Table Lookup Efficiency with SoA Layout

<!-- md-trans-meta sourceCommit=1fa0fc4f3ef074ae5db86a4cd38e1778da419dc2 translatedAt=2026-11-09T10:42:21.443Z pushedAt=2026-11-19T06:32:19.832Z nl=2 -->

[Priority] Medium

[Description] AoS (Array of Structures) and SoA (Structure of Arrays) are two different memory layouts:
- AoS layout: Each structure contains the complete information of one object, and multiple structures are arranged into an array and stored in memory.
- SoA layout: The same type of information of objects is arranged into an array, and the same type of information of multiple objects is stored in adjacent memory.

[Sample introduction] Take the [hashtable_lookup sample](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/hashtable_lookup) as an example. It implements a hash table lookup operator. The hash table resolves hash collisions based on open addressing. For details, see [Background Knowledge](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table). This sample looks up the hash table through different hash table structure designs and thread access methods and performs a performance comparison, demonstrating the impact of different optimization methods on performance.

The following figure shows the hash table structures with different memory layouts in this sample.

**Figure 1** Memory layout diagram

![Memory layout diagram](../../../figures/soa_aos_layout.png)

<!-- npu="950" id1 -->
The performance data below is collected using the msOpProf tool on Ascend 950PR products, with the metric being Task Duration (the overall task duration, that is, the end-to-end operator duration).
<!-- end id1 -->

[Negative example] In the AoS memory layout scenario, an 8B key and a 32B value are arranged contiguously to form a bucket structure. Multiple buckets are arranged contiguously to form an array. The lookup logic reads the 8B key bucket by bucket for comparison. Each thread independently handles one query key.

The hash table structure code is implemented as follows:

```cpp
struct AoSBucket {
  uint64_t key;
  float value[DIM];
};
AoSBucket table[CAPACITY];
__aicore__ inline uint32_t find_aos(const AoSBucket* table, uint64_t target_key, uint32_t start) {
  ...
  const uint64_t bucket_key = table[idx].key;
  if (bucket_key == target_key) {
      return idx;  
  }
  if (bucket_key == EMPTY_KEY) {
      return NOT_FOUND;  
  }
  idx = (idx + 1) & (CAPACITY - 1);  
  ...
}
```

In the preceding implementation, each thread independently handles one query key, and the lookup logic reads the 8B key bucket by bucket for comparison. The performance data of this implementation is as follows:

| Case | Task Duration (μs) |
| ---- | :---------------: |
| 0    |     1311.297      |


[Positive example] With the SoA layout, keys and values are stored in separate regions, and the probe path is decoupled from the value region, eliminating the interference of values stored in buckets on the probe memory access span.

```cpp
struct SoaTable {
  uint64_t key[CAPACITY];
  float value[CAPACITY][DIM];
};
SoaTable table;
__aicore__ inline uint32_t find_soa(const SoaTable* table, uint64_t target_key, uint32_t start) {
  ...
  const uint64_t bucket_key = table->key[idx];
  if (bucket_key == target_key) {
      return idx;  
  }
  if (bucket_key == EMPTY_KEY) {
      return NOT_FOUND;  
  }
  idx = (idx + 1) & (CAPACITY - 1);  
  ...
}
```

The algorithm implementation is exactly the same as the lookup algorithm in the negative example. The only difference is the memory layout. In the SoA layout, all 8 MB keys are stored contiguously, all 32 MB values are stored contiguously, and keys and values are stored separately. The performance data after using the SoA layout is as follows:

| Case | Task Duration (μs) | Performance Comparison |
| ---- | :---------------: | :------: |
| 0    |     1311.297      |    -     |
| 1    |      783.029      |  1.67x   |

As shown in the performance data, for the same algorithm, the task duration decreases from 1311.297 μs to 783.029 μs. The SoA layout achieves a 1.67x improvement over the AoS layout.

**Optimization effect analysis**:

In AoS, keys and values are stored alternately, so querying a key also loads the adjacent value. In SoA, all keys are stored contiguously, and a single 128B cache line can hold more keys, for example, 16 8B keys, thereby reducing unnecessary value data loading during lookup, lowering the cache line transfer overhead, and improving the cache utilization rate.

**Extended applicable scenarios**

The SoA layout is not limited to hash table lookup: when an operator involves batch access to the same field of local data, prefer SoA over AoS, store fields of the same type contiguously, reduce the loading of irrelevant data, and improve the cache line utilization rate. For details, see the [hash table lookup performance optimization sample](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/hashtable_lookup/README.md).

[Summary] In SIMT programming, when the SoA memory layout is used, fields of the same type are stored contiguously. For scenarios with high-frequency access to local data, the SoA memory layout can significantly improve the cache line utilization rate, thereby improving overall performance.
