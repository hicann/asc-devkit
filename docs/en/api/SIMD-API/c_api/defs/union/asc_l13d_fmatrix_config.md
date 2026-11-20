---
okftype: reference.union
description: "Union for setting the Feature map attributes of the 3D format data movement operations asc_copy_l12l0a and asc_copy_l12l0b. The fields are l1_width, l1_height, and the padding size on all four sides in order. You must assign values to l1_height and l1_width before use."
tag:
  - roles: [data_movement_configuration]
  - operations: [img2col]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Fixed the issue where the incorrect order of the l1_width and l1_height fields caused the configured value to be inconsistent with the expected value; the fix item on line 106 of the CANN_9_2_0 Release Notes, applicable to scenarios where this struct is used to configure the Feature map width and height."
---
# asc_l13d_fmatrix_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:17:53.354Z pushedAt=2026-11-18T08:15:34.413Z nl=2 -->

The header file path is: `"c_api/defs/union.h"`.

asc_l13d_fmatrix_config is used to set the Feature map attribute parameters of the 3D format copy interfaces of [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a.md)/[asc_copy_l12l0b](../../cube_datamove/cube_compute_load/asc_copy_l12l0b.md).

## Struct Definition

```cpp
constexpr uint64_t ASC_DEFAULT_L13D_FMATRIX_CONFIG_VALUE = 0;
union asc_l13d_fmatrix_config {
    uint64_t config = ASC_DEFAULT_L13D_FMATRIX_CONFIG_VALUE;
    struct {
        uint16_t l1_width;
        uint16_t l1_height;
        uint8_t padding_left_size;
        uint8_t padding_right_size;
        uint8_t padding_top_size;
        uint8_t padding_bottom_size;
    };
};
```

## Constraints

After a union variable of the `asc_l13d_fmatrix_config` type is defined, you must assign values to `l1_height` and `l1_width` to meet the actual scenario and value range requirements.

## Field Description

**Table 1** Field description

|Field Name|Field Meaning|
|----------|----------|
| l1_width | Width of the Feature map. Value range: [1, 32767]. |
| l1_height | Height of the Feature map. Value range: [1, 32767]. |
| padding_left_size | Size of the left padding. Value range: [0, 255]. |
| padding_right_size | Size of the right padding. Value range: [0, 255]. |
| padding_top_size | Size of the top padding. Value range: [0, 255]. |
| padding_bottom_size | Size of the bottom padding. Value range: [0, 255]. |
