---
okftype: reference.api
description: "废弃接口页：asc_set_l0c2gm_config 以 relu_pre、quant_pre 起始地址与 enable_unit_flag 三个参数设置随路量化在矢量化流程中的参数，量化参数不可为 inf 或 NaN，建议改用 asc_set_l0c_copy_config。"
tag:
  - roles: [configuration]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "随路量化时由该搬出接口消费本配置"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 要求改用函数名与参数名调整后的新接口"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 废弃表把 asc_set_l0c2gm_config 列为废弃，替代为 asc_set_l0c_copy_config；接口仍保留可用。"
---
# asc_set_l0c2gm_config（废弃）

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
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

**`asc_set_l0c2gm_config`接口已废弃。请使用[asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md)接口替代。**

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

数据搬运过程中进行随路量化时，通过调用该接口设置量化流程中的矢量量化参数。

## 函数原型

```cpp
__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre, uint64_t quant_pre, bool enable_unit_flag)
```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| relu_pre | 输入     | ReLU操作前矢量的起始地址。|
| quant_pre | 输入     | 量化操作前矢量的起始地址。|
| enable_unit_flag | 输入     | 是否启用unit_flag。unit_flag是一种矩阵计算指令和矩阵搬运指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出，该功能不适用于L0C Buffer累加的场景。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

量化参数不能为inf/nan和非规格化数。

支持以下三种传参形式：
- 同时设置relu_pre和quant_pre。
- 仅传入relu_pre，quant_pre传入0。
- 仅传入quant_pre，relu_pre传入0。

## 调用示例

```cpp
constexpr uint64_t relu_pre = 0;
constexpr uint64_t quant_pre = 0x1000;// 假设量化操作有效地址为0x1000
asc_set_l0c2gm_config(relu_pre, quant_pre, true);
```
