---
okftype: reference.enum
description: "Enum for the L2 Cache policy when data is copied from the Unified Buffer or L0C Buffer to GM. It is used by the configuration and query interfaces of vector copy, cube copy-out with L2 configuration support, and the scalar memory access path. It includes three NORMAL policies and NOTALLOC_CLEAN."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_get_scalar_store_cache_mode.md
    kind: related
    condition: "This interface returns the current store policy represented by this enum"
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "This interface sets the store policy of the scalar memory access path using this enum"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: related
    condition: "This 3510 cube copy-out interface configures the L2 Cache using this enum"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md
    kind: related
    condition: "This vector data movement interface uses this enum to configure the store-side L2 Cache policy"
version: ">=9.2.0"
version_changes: []
---
# asc_store_l2_cache_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:13:41.122Z pushedAt=2026-11-18T08:15:34.403Z nl=2 -->

asc_store_l2_cache_mode is used to indicate the L2 Cache policy when data is moved from the Unified Buffer (UB) or L0C Buffer to GM. It can be used for:

- Vector data movement interfaces, for example, [asc_copy_ub2gm_align](../../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md).
- Cube move-out interfaces that support L2 Cache configuration, for example, [asc_copy_l0c2gm](../../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) under NPU architecture 3510.
- Store policy configuration and query for the scalar memory access path, for example, [asc_set_scalar_cache_mode](../../cache_ctrl/asc_set_scalar_cache_mode.md) and [asc_get_scalar_store_cache_mode](../../cache_ctrl/asc_get_scalar_store_cache_mode.md).

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_store_l2_cache_mode : uint8_t {
    NORMAL_FIRST_VICTIM = 0,
    NORMAL_LAST_VICTIM = 1,
    NORMAL_PERSISTENT = 2,
    NOTALLOC_CLEAN = 4,
    WT_SHARE_FIRST_VICTIM = 12
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | L2 Cache Policy |
| ------ | ----------- |
| NORMAL_FIRST_VICTIM | Enables L2 Cache and marks the allocated Cache Line with high replacement priority (FIRST_VICTIM), meaning it is evicted first when replacement occurs. |
| NORMAL_LAST_VICTIM | Enables L2 Cache and marks the allocated Cache Line with low replacement priority (LAST_VICTIM), meaning it is retained first when replacement occurs. |
| NORMAL_PERSISTENT | Enables L2 Cache. Data already stored in L2 Cache may be replaced. To ensure specific data always remains in L2 Cache, the persistent mode can be used.<br>&bull; Note that a Cache Line marked as persistent can only be replaced by another Cache Line that is also marked as persistent. |
| NOTALLOC_CLEAN | Does not enable L2 Cache. If a cache with the same address already exists in L2 Cache, it is retained and marked as Clean. A Cache Line marked as Clean is directly discarded when evicted from L2 Cache. |
| WT_SHARE_FIRST_VICTIM | Enables L2 Cache, adopts the write-through policy, and marks newly allocated Cache Lines with high replacement priority (FIRST_VICTIM), meaning they are evicted first when replacement occurs.<br>write-through means that when data is written, GM and L2 Cache are updated synchronously. The Cache Line does not need to be marked as Dirty, nor does it need to wait for the Cache Line to be replaced or a Clean operation to be performed before writing back to GM.<br>share means that Cache Lines allocated using this mode are in the Shared state. In the MESI cache coherence protocol, Shared indicates that the Cache Line is in the Clean state, its data is consistent with the next-level storage, and other caches are allowed to hold valid copies of the same address. |

Clean and Dirty are used to indicate whether a Cache Line needs to be written back to GM:

- Clean: The Cache Line is not marked as requiring write-back. It can be directly discarded when replaced, without writing back to GM.
- Dirty: The Cache Line contains modified data that has not yet been written back to GM. When replaced, the modified data must first be written back to GM.

A Clean operation means writing the modified data in a Dirty Cache Line back to GM and marking it as Clean.
