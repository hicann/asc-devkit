---
okftype: reference.api
description: "已废弃的地址寄存器初始化接口族，按 b8、b16、b32 位宽创建可存放 1 至 4 个偏移量的地址寄存器，供循环内多维迭代寻址，替代方案为 asc_update_addr_reg。"
tag:
  - roles: [configuration]
  - traits: [high_dimensional]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "地址寄存器的类型定义与调用示例所在页面"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: replaced_by
    condition: "正文声明的替代接口，按 b8/b16/b32 分别对应"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "9.1.0 起三个位宽版本的地址寄存器初始化接口由单个 offset 扩展为支持 1 至 4 个 offset 参数（Release Notes CANN_9_1_0 记录该扩展），用于适配 1 至 4 维 Tensor 的循环寻址；9.0.0 只有单参数形式。"
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "9.2.0 起 include 对三个位宽的重载逐条标记 ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_update_addr_reg_b32/b16/b8)，Release Notes CANN_9_2_0 的替代关系表说明地址寄存器类型与初始化接口统一更名为 addr_reg 与 asc_update_addr_reg_b8/b16/b32；声明保留，属 deprecated 而非 removed。"
---
# asc_create_iter_reg（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
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

头文件路径为：`"c_api/reg_compute/compute/reg_addr_reg.h"`。

**该接口已废弃，请使用[asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md)实现此功能。**

地址寄存器通过该接口初始化，然后在循环之中使用地址寄存器存储地址偏移量，该重载接口同步支持1-4个offset参数，并支持初始化b8/b16/b32位宽的寄存器，适配1-4维Tensor。底层在多维循环中自动计算地址偏移，消除标量地址运算开销。

## 函数原型

  ```cpp
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  ```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| offset       | 输入 | 地址偏移量。 |
| offset0      | 输入 | 地址偏移量。 |
| offset1     | 输入 | 地址偏移量。 |
| offset2     | 输入 | 地址偏移量。 |
| offset3     | 输入 | 地址偏移量。 |

## 返回值说明

地址寄存器。

## 流水类型

PIPE_V

## 约束说明

- 请勿在循环内使用 if/else。
- iter_reg必须在最内层循环体内赋值使用。
- 不同步长的操作数，建议使用多个iter_reg，避免地址被覆盖。

## 调用示例

见[地址寄存器调用示例](../defs/type/data_type_definition.md#example4)。
