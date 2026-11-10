---
okftype: reference.union
description: "为三维载入接口设置 repeat 迭代参数的联合体，位域 rpt_stride、rpt_time、rpt_mode 分别描述 height 或 width 方向相邻迭代起始地址的距离、迭代次数与迭代方向，默认 config 值为 0x0000000000010000。"
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

头文件路径为：`"c_api/defs/union.h"`。

asc_load3d_v2_config用于设置Load3Dv2接口的repeat参数。

## 结构体具体定义

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

## 字段详解

**表1** 字段说明

|字段名|字段含义|
|----------|----------|
| rpt_stride | height/width方向上的前一个迭代与后一个迭代起始地址的距离，取值范围：[0, 65535]，默认值为0。 |
| rpt_time | height/width方向上的迭代次数，取值范围：[0, 255]，默认值为1。 |
| rpt_mode | 控制repeat迭代的方向，取值范围：[0, 1]，默认值为0。<br>0：迭代沿height方向；<br>1：迭代沿width方向。 |
