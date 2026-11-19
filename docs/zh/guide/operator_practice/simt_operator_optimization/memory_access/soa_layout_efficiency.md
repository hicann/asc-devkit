# 使用SoA布局提升哈希表查找效率

【优先级】中

【描述】AoS（Array of Structures，结构体数组）与SoA（Structure of Arrays，数组结构体）为两种不同内存布局：

- AoS布局：每个结构体中包含一个对象的完整信息，多个结构体排列成数组存储在内存中。
- SoA布局：将对象的同种信息排列成数组，多个对象的同种信息存储在相邻内存中。

【样例介绍】以[hashtable_lookup样例](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/hashtable_lookup)为例，实现哈希表查找算子，该哈希表处理哈希冲突基于开放寻址法，具体参考[背景知识](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table)，此样例通过不同的哈希表结构设计以及线程访问方式查找哈希表并进行性能比较，展示不同优化手段对性能的影响。

下图展示了本样例中不同内存布局的哈希表结构。

**图1 内存布局示意图**

![内存布局示意图](../../../figures/soa_aos_layout.png)

<!-- npu="950" id1 -->
下文性能数据均使用msOpProf工具在Ascend 950PR产品上采集，指标为Task Duration（Task整体耗时，即算子端到端耗时）。
<!-- end id1 -->

【反例】AoS内存排布场景，8B的key和32B的value连续排布，组成一个Bucket结构，多个Bucket连续排布组成一个数组，查找逻辑逐Bucket读8B key进行比较。每个线程独立负责一条查询key。

哈希表结构体代码实现如下所示：

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

上述实现中，每个线程独立负责一条查询key，查找逻辑逐Bucket读8B key进行比较。该实现的性能数据如下：

| Case | Task Duration(μs) |
| ---- | :---------------: |
| 0    |     1311.297      |

【正例】使用SoA布局，key与value分区存储，探测路径与value区解耦，消除value存放在Bucket中对探测访存跨度的干扰。

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

算法实现与反例查找算法完全一致。唯一差异是内存布局。SoA布局中所有8MB的key连续存放，所有32MB的value连续存放，key和value分开存储。使用SoA布局后的性能数据如下：

| Case | Task Duration(μs) | 性能对比 |
| ---- | :---------------: | :------: |
| 0    |     1311.297      |    -     |
| 1    |      783.029      |  1.67x   |

从性能数据可以看出，同一算法，Task Duration从1311.297μs降至783.029μs。SoA布局相对AoS布局提升x1.67。

**优化效果分析**：

AoS中key和value交替存储，查询key时会将相邻的value一同加载；SoA中所有key连续存储，单条128B Cache Line可容纳更多key，例如16个8B的key，从而减少查找过程中不必要的value数据加载，降低Cache Line搬运开销，提高缓存利用率。

**适用场景扩展**

SoA布局不局限于哈希表查找：当算子存在局部同字段批量访问时，优先考虑SoA而非AoS，将同类字段连续存储，减少无关数据的加载，提高Cache Line利用率，详见[哈希表查找性能优化样例](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/hashtable_lookup/README.md)。

【总结】在SIMT编程中，使用SoA的内存布局时同类字段连续存储，针对高频访问局部数据的场景，SoA的内存排布可显著提高Cache Line利用率，从而提升整体性能。
