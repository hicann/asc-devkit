# asc_load_l2_cache_mode

asc_load_l2_cache_mode用于表示数据从GM搬运到UB时的L2 cache管理策略，在调用[asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)和[asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md)时使用。L2 Cache管理策略的影响可见[L2 Cache Mode最佳实践](../../../../../../examples/02_simd_c_api/02_features/03_c_api/01_data_movement/00_set_l2_cache_mode/README.md)。

## 枚举类具体定义

```cpp
enum class asc_load_l2_cache_mode : uint8_t {
    NORMAL_FIRST_VICTIM = 0,
    NORMAL_LAST_VICTIM = 1,
    NORMAL_PERSISTENT = 2,
    NOTALLOC_KEEP = 4,
    NOTALLOC_CLEAN = 5,
    NOTALLOC_DROP = 6
};
```

## 枚举值详解

|枚举值|L2 cache管理策略|
|------|-----------|
| NORMAL_FIRST_VICTIM | 启用L2 Cache，并且将分配的Cache Line标记为高替换优先级。 |
| NORMAL_LAST_VICTIM | 启用L2 Cache，并且将分配的Cache Line标记为低替换优先级。 |
| NORMAL_PERSISTENT | 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。 |
| NOTALLOC_KEEP | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |
| NOTALLOC_CLEAN | 不启用L2 Cache，若L2 Cache中已有同地址缓存会被保留并标记为Clean，标记为Clean的Cache Line被移出L2 Cache时会被直接丢弃。 |
| NOTALLOC_DROP | 不启用L2 Cache，若L2 Cache中已有同地址缓存，会被直接丢弃。 |
