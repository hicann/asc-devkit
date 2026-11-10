---
okftype: reference.enum
description: "数据从Unified Buffer或L0C Buffer搬运到GM时的L2 Cache策略枚举，供向量搬运、支持L2配置的矩阵搬出以及标量访存路径的配置与查询接口使用，含三种NORMAL策略与NOTALLOC_CLEAN。"
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_get_scalar_store_cache_mode.md
    kind: related
    condition: "该接口返回本枚举表示的当前store策略"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "该接口以本枚举设置标量访存路径的store策略"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: related
    condition: "该3510矩阵搬出接口以本枚举配置L2 Cache"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md
    kind: related
    condition: "该向量搬运接口以本枚举配置store侧L2 Cache策略"
version: ">=9.2.0"
version_changes: []
---
# asc_store_l2_cache_mode

asc_store_l2_cache_mode用于表示数据从Unified Buffer（UB）或L0C Buffer搬运到GM时的L2 Cache策略，可用于：

- 向量搬运接口，例如[asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md)。
- 支持L2 Cache配置的矩阵搬出接口，例如NPU架构3510下的[asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md)。
- 标量访存路径的store策略配置与查询，例如[asc_set_scalar_cache_mode](../../cache_ctrl/asc_set_scalar_cache_mode.md)和[asc_get_scalar_store_cache_mode](../../cache_ctrl/asc_get_scalar_store_cache_mode.md)。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举类具体定义

```cpp
enum class asc_store_l2_cache_mode : uint8_t {
    NORMAL_FIRST_VICTIM = 0,
    NORMAL_LAST_VICTIM = 1,
    NORMAL_PERSISTENT = 2,
    NOTALLOC_CLEAN = 4,
    WT_SHARE_FIRST_VICTIM = 12
};
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | L2 Cache策略 |
| ------ | ----------- |
| NORMAL_FIRST_VICTIM | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级（FIRST_VICTIM），即在发生替换时优先被淘汰。 |
| NORMAL_LAST_VICTIM | 启用L2 Cache，并将分配的Cache Line标记为低替换优先级（LAST_VICTIM），即在发生替换时优先保留。 |
| NORMAL_PERSISTENT | 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定数据始终保留在L2 Cache中，可采用驻留模式。<br>&bull; 注意，被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。 |
| NOTALLOC_CLEAN | 不启用L2 Cache，若L2 Cache中已有同地址缓存会被保留并标记为Clean，标记为Clean的Cache Line被移出L2 Cache时会被直接丢弃。 |
| WT_SHARE_FIRST_VICTIM | 启用L2 Cache，采用write-through（直写）策略，并将新分配的Cache Line标记为高替换优先级（FIRST_VICTIM），即在发生替换时优先被淘汰。<br>write-through表示写入数据时同步更新GM和L2 Cache，Cache Line无需标记为Dirty，也无需等待Cache Line被替换或执行Clean操作后再回写GM。<br>share表示使用本模式分配的Cache Line处于Shared（共享）状态。在MESI缓存一致性协议中，Shared表示该Cache Line为Clean状态，数据与下一级存储保持一致，并允许其他缓存持有同一地址的有效副本。 |

Clean和Dirty用于表示Cache Line是否需要回写GM：

- Clean（干净）：该Cache Line未被标记为需要回写，被替换时可直接丢弃，无需回写GM。
- Dirty（脏）：该Cache Line包含尚未回写GM的修改数据，被替换时需要先将修改数据回写GM。

Clean操作表示将Dirty Cache Line中的修改数据回写GM，并将其标记为Clean。
