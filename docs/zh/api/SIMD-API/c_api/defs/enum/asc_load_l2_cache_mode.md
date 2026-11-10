---
okftype: reference.enum
description: "数据从GM搬运到Unified Buffer或L1 Buffer时的L2 Cache管理策略枚举，供向量搬运、支持L2配置的矩阵搬入以及标量访存路径的配置与查询接口使用，区分NORMAL启用与NOTALLOC不启用两类策略。"
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_get_scalar_load_cache_mode.md
    kind: related
    condition: "该接口返回本枚举表示的当前load策略"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "该接口以本枚举设置标量访存路径的load策略"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md
    kind: related
    condition: "该3510矩阵搬入接口使用本枚举配置L2 Cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: related
    condition: "该矩阵搬入接口的重载使用本枚举配置L2 Cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md
    kind: related
    condition: "该矩阵搬入接口使用本枚举配置L2 Cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md
    kind: related
    condition: "该3510矩阵搬入接口使用本枚举配置L2 Cache"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md
    kind: related
    condition: "该向量搬运接口以本枚举配置L2 Cache策略"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: related
    condition: "该向量搬运接口以本枚举配置L2 Cache策略"
version: ">=9.2.0"
version_changes: []
---
# asc_load_l2_cache_mode

asc_load_l2_cache_mode用于表示数据从GM搬运到Unified Buffer（UB）或L1 Buffer时的L2 Cache管理策略，可用于：

- 向量搬运接口，例如[asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)和[asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md)。
- 支持L2 Cache配置的矩阵搬入接口，例如NPU架构3510下的[asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md)、[asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md)、[asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md)和[asc_copy_gm2l1_dn2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md)。
- 标量访存路径的load策略配置与查询，例如[asc_set_scalar_cache_mode](../../cache_ctrl/asc_set_scalar_cache_mode.md)和[asc_get_scalar_load_cache_mode](../../cache_ctrl/asc_get_scalar_load_cache_mode.md)。

L2 Cache管理策略的影响可见[L2 Cache Mode最佳实践](../../../../../../../examples/02_simd_c_api/02_features/00_data_movement/00_set_l2_cache_mode/README.md)。

头文件路径为：`"c_api/defs/enum.h"`。

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

**表1** 枚举值说明

|枚举值|L2 Cache管理策略|
|------|-----------|
| NORMAL_FIRST_VICTIM | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级（FIRST_VICTIM），即在发生替换时优先被淘汰。 |
| NORMAL_LAST_VICTIM | 启用L2 Cache，并将分配的Cache Line标记为低替换优先级（LAST_VICTIM），即在发生替换时优先保留。 |
| NORMAL_PERSISTENT | 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。 |
| NOTALLOC_KEEP | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |
| NOTALLOC_CLEAN | 不启用L2 Cache，若L2 Cache中已有同地址缓存会被保留并标记为Clean，标记为Clean的Cache Line被移出L2 Cache时会被直接丢弃。 |
| NOTALLOC_DROP | 不启用L2 Cache，若L2 Cache中已有同地址缓存，会被直接丢弃。 |

Clean和Dirty用于表示Cache Line是否需要回写GM：

- Clean（干净）：该Cache Line未被标记为需要回写，被替换时可直接丢弃，无需回写GM。
- Dirty（脏）：该Cache Line包含尚未回写GM的修改数据，被替换时需要先将修改数据回写GM。
