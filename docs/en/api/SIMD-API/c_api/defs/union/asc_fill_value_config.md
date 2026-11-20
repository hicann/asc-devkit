---
okftype: reference.union
description: "A union that provides parameters for the asc_fill_l0a, asc_fill_l0b, and asc_fill_l1 initialization interfaces. The config bit-field carries repeat, blk_num, and dst_gap. When the destination is in L1, the block size and gap are interpreted in units of 32 bytes; when in L0A or L0B, they are interpreted in units of 512 bytes."
tag:
  - roles: [data_movement_configuration]
  - operations: [fill]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_fill_l0a.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_fill_l0b.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_fill_l1.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_fill_value_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:17:12.920Z pushedAt=2026-11-18T08:15:34.412Z nl=2 -->

The header file path is: `"c_api/defs/union.h"`.

asc_fill_value_config is the initialization parameter structure for fill_value, containing the various initialization parameters required by the [asc_fill_l0a](../../cube_datamove/cube_load_aux_config/asc_fill_l0a.md)/[asc_fill_l0b](../../cube_datamove/cube_load_aux_config/asc_fill_l0b.md)/[asc_fill_l1](../../cube_datamove/cube_load_aux_config/asc_fill_l1.md) interfaces.

## Structure Specific Definition

```cpp
constexpr uint64_t ASC_FILL_VALUE_DEFAULT_VALUE = 0x0000000000000000;
union asc_fill_value_config {
    uint64_t config = ASC_FILL_VALUE_DEFAULT_VALUE;
    struct {
        uint64_t repeat : 15;
        uint64_t reserved1 : 1;
        uint64_t blk_num : 15;
        uint64_t reserved2 : 1;
        uint64_t dst_gap : 15;
        uint64_t reserved3 : 17;
    };
};
```

## Field Details

**Table 1** Field description

|Field Name|Field Meaning|
|----------|----------|
| repeat | Number of iterations. Default Value: 0. Value Range: [0, 32767]. |
| blk_num | Number of data blocks initialized in each iteration. Default Value: 0. Value Range: [0, 32767].<br>When the target operand is in the L1 Buffer, the data block size is 32B; when the target operand is in the L0A Buffer/L0B Buffer, the data block size is 512B. |
| dst_gap | Distance between the end address of the previous iteration and the start address of the next iteration of the target operand. Default Value: 0. Value Range: [0, 32767].<br>When the target operand is in the L1 Buffer, the unit is 32B; when the target operand is in the L0A Buffer/L0B Buffer, the unit is 512B. |