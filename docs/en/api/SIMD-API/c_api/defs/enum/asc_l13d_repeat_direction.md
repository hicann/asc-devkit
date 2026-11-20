---
okftype: reference.enum
description: "Enum of repeat iteration directions for 3D img2col data movement, used as the repeat_direction parameter type of asc_set_l13d_rpt and asc_set_l13d_rpt_b, distinguishing the M direction along height from the K direction along width, and determining the unit of repeat_stride."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l13d_rpt.md
    kind: related
    condition: "This interface uses this enum as the repeat_direction parameter"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l13d_rpt_b.md
    kind: related
    condition: "This interface uses this enum as the repeat_direction parameter"
version: ">=9.2.0"
version_changes: []
---
# asc_l13d_repeat_direction

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:09:37.933Z pushedAt=2026-11-18T08:15:34.389Z nl=2 -->

`asc_l13d_repeat_direction` is used to represent the repeat iteration direction of 3D img2col data movement, and serves as the `repeat_direction` parameter type of the [asc_set_l13d_rpt](../../cube_datamove/cube_load_aux_config/asc_set_l13d_rpt.md) and [asc_set_l13d_rpt_b](../../cube_datamove/cube_load_aux_config/asc_set_l13d_rpt_b.md) interfaces.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_l13d_repeat_direction : uint8_t {
    M_DIRECTION = 0,
    K_DIRECTION = 1
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum value | Value | Repeat iteration direction | `repeat_stride` unit |
| --- | --- | --- | --- |
| M_DIRECTION | 0 | Iterates along the height (M) direction. | 16 elements. |
| K_DIRECTION | 1 | Iterates along the width (K) direction. | `32 / sizeof(data_type)` elements, where `data_type` is the data type of the source operand of the 3D transfer interface. |
