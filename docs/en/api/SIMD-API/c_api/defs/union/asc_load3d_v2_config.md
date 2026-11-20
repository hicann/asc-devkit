---
okftype: reference.union
description: "Union for setting repeat iteration parameters for the 3D load interface. The bitfields rpt_stride, rpt_time, and rpt_mode respectively describe the distance between the start addresses of adjacent iterations in the height or width direction, the number of iterations, and the iteration direction. The default config value is 0x0000000000010000."
tag:
  - roles: [data_movement_configuration]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l13d_rpt.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_load3d_v2_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:18:15.593Z pushedAt=2026-11-18T08:15:34.416Z nl=2 -->

The header file path is: `"c_api/defs/union.h"`.

asc_load3d_v2_config is used to set the repeat parameter of the Load3Dv2 interface.

## Structure Detailed Definition

```cpp
constexpr uint64_t ASC_DEFAULT_LOAD3D_V2_CONFIG_VALUE = 0x0000000000010000;
union asc_load3d_v2_config {
    uint64_t config = ASC_DEFAULT_LOAD3D_V2_CONFIG_VALUE;
    struct {
        uint64_t rpt_stride : 16;
        uint64_t rpt_time : 8;
        uint64_t rpt_mode : 1;
        uint64_t reserved2 : 39;
    };
};
```

## Field Details

**Table 1** Field description

|Field Name|Field Meaning|
|----------|----------|
| rpt_stride | Distance between the start addresses of the previous iteration and the next iteration in the height/width direction. Value Range: [0, 65535]. Default Value: 0. |
| rpt_time | Number of iterations in the height/width direction. Value Range: [0, 255]. Default Value: 1. |
| rpt_mode | Controls the direction of repeat iteration. Value Range: [0, 1]. Default Value: 0.<br>0: Iteration along the height direction;<br>1: Iteration along the width direction. |
