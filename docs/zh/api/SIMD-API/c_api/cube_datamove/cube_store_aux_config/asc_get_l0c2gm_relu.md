---
okftype: reference.api
description: "读取 L0C Buffer 搬出随路 ReLU 激活时使用的激活参数起始地址，返回 FPC 寄存器中由 asc_set_l0c_copy_config 写入的 relu_pre_addr 配置字段，而非激活计算后的数据。"
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: related
    condition: "返回的地址索引由该接口的 relu_pre_addr 参数配置"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_relu.md
    kind: related
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "9.1.0 的声明位于 include/c_api/cube_compute/cube_compute.h，9.2.0 起迁到 include/c_api/cube_datamove/cube_datamove.h 并延续到 9.3.0-beta.1，签名保持无参返回 uint64_t 不变。"
---
# asc_get_l0c2gm_relu

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

数据搬运过程中进行随路量化时，通过调用该接口获取ReLU操作前矢量的起始地址。

激活模式及配套配置见[随路ReLU](../cube_store_key_features/accompanying_relu.md)；与量化同时使用时，参见[随路量化与随路ReLU场景组合](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md)。

该状态由[asc_set_l0c_copy_config](asc_set_l0c_copy_config.md)配置，其`relu_pre_addr`参数的含义和配置覆盖行为参见对应接口。返回值是FPC寄存器中的配置字段，不是量化或激活计算后的数据。

## 函数原型

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_relu()
```

## 参数说明

无

## 返回值说明

ReLU操作前矢量的起始地址。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
uint64_t relu_addr = asc_get_l0c2gm_relu();
```
