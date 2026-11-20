---
okftype: reference.enum
description: "Enum of L2 cache management policies for data copied from GM to Unified Buffer or L1 Buffer. It is used by the configuration and query interfaces of vector data movement, cube copy-in that supports L2 configuration, and the scalar memory access path, distinguishing two policy categories: NORMAL enabled and NOTALLOC disabled."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_get_scalar_load_cache_mode.md
    kind: related
    condition: "This interface returns the current load policy represented by this enum"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "This interface sets the load policy of the scalar memory access path with this enum"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md
    kind: related
    condition: "This 3510 cube copy-in interface uses this enum to configure the L2 cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md
    kind: related
    condition: "The overload of this cube copy-in interface uses this enum to configure the L2 cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md
    kind: related
    condition: "This cube copy-in interface uses this enum to configure the L2 cache"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md
    kind: related
    condition: "This 3510 cube copy-in interface uses this enum to configure the L2 cache"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md
    kind: related
    condition: "This vector copy interface uses this enum to configure the L2 cache policy"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: related
    condition: "This vector copy interface uses this enum to configure the L2 cache policy"
version: ">=9.2.0"
version_changes: []
---
# asc_load_l2_cache_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:10:19.200Z pushedAt=2026-11-18T08:15:34.392Z nl=2 -->

asc_load_l2_cache_mode is used to represent the L2 cache management policy when data is copied from GM to Unified Buffer (UB) or L1 Buffer. It can be used for:

- Vector copy interfaces, such as [asc_copy_gm2ub_align](../../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) and [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md).
- Cube copy-in interfaces that support L2 cache configuration, such as [asc_copy_gm2l1_align](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md), [asc_copy_gm2l1](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md), [asc_copy_gm2l1_nd2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md), and [asc_copy_gm2l1_dn2nz](../../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md) under NPU architecture 3510.
- Load policy configuration and query for the scalar memory access path, such as [asc_set_scalar_cache_mode](../../cache_ctrl/asc_set_scalar_cache_mode.md) and [asc_get_scalar_load_cache_mode](../../cache_ctrl/asc_get_scalar_load_cache_mode.md).

For the impact of the L2 cache management policy, see [L2 Cache Mode Best Practices](../../../../../../../examples/02_simd_c_api/02_features/00_data_movement/00_set_l2_cache_mode/README.md).

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

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

## Enum Value Details

**Table 1** Enum value description

|Enum Value|L2 Cache Management Policy|
|------|-----------|
| NORMAL_FIRST_VICTIM | L2 cache is enabled, and the allocated cache line is marked with high replacement priority (FIRST_VICTIM), meaning it is evicted first when replacement occurs. |
| NORMAL_LAST_VICTIM | L2 cache is enabled, and the allocated cache line is marked with low replacement priority (LAST_VICTIM), meaning it is retained first when replacement occurs. |
| NORMAL_PERSISTENT | L2 cache is enabled. Data already stored in the L2 cache may be replaced. To ensure specific data always remains in the L2 cache, the persistent mode can be used.<br>&bull; Note that a cache line marked as persistent can only be replaced by other cache lines that are also marked as persistent. |
| NOTALLOC_KEEP | L2 cache is disabled. Data is read directly from GM each time, and the state of existing cache lines remains unchanged. |
| NOTALLOC_CLEAN | L2 cache is disabled. If a cache line with the same address already exists in the L2 cache, it is retained and marked as Clean. A cache line marked as Clean is directly discarded when it is evicted from the L2 cache. |
| NOTALLOC_DROP | L2 cache is disabled. If a cache with the same address already exists in the L2 cache, it is directly discarded. |

Clean and Dirty are used to indicate whether a cache line needs to be written back to GM:

- Clean: The cache line is not marked as requiring write-back. It can be directly discarded when replaced, without write-back to GM.
- Dirty: The cache line contains modified data that has not yet been written back to GM. When replaced, the modified data must first be written back to GM.
