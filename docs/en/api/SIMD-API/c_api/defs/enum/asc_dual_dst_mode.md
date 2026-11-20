---
okftype: reference.enum
description: "Enum of dual-destination split modes for L0C Buffer data move-out, used as the type of the dual_dst_ctrl parameter of the cube move-out interface. It can take three values: disable dual destination, split along the M direction, or split along the N direction."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2ub.md
    kind: related
    condition: "This cube move-out interface uses this enum as the dual_dst_ctrl parameter"
version: ">=9.2.0"
version_changes: []
---
# asc_dual_dst_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:09:20.254Z pushedAt=2026-11-18T08:15:34.385Z nl=2 -->

`asc_dual_dst_mode` is used to represent the dual-destination split mode when moving L0C Buffer data out, and serves as the type of the `dual_dst_ctrl` parameter in the cube move-out interface.

The header file path is: `"c_api/defs/enum.h"`.

## Detailed Definition of the Enum Class

```cpp
enum class asc_dual_dst_mode : uint8_t {
    DUAL_DST_DISABLE = 0,
    DUAL_DST_SPLIT_M = 1,
    DUAL_DST_SPLIT_N = 2
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Value | Description |
| --- | --- | --- |
| DUAL_DST_DISABLE | 0 | Disable dual-destination mode. |
| DUAL_DST_SPLIT_M | 1 | Enable dual-destination mode and split data along the M direction. |
| DUAL_DST_SPLIT_N | 2 | Enable dual-destination mode and split data along the N direction. |
