---
okftype: reference.api
description: "废弃接口页：asc_set_l0c2gm_lrelu_alpha 以 half 或 float 引用设置 asc_copy_l0c2l1 与 asc_copy_l0c2gm 使用的 Leaky ReLU alpha 值，建议改用仅提供 float 形式的 asc_set_l0c_copy_lrelu_alpha。"
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "随路 Leaky ReLU 时由该搬出接口消费 alpha 值"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "随路 Leaky ReLU 时由该搬出接口消费 alpha 值"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 要求改用新接口，新接口仅支持 float 参数"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 废弃表把 asc_set_l0c2gm_lrelu_alpha 的 half& 与 float& 重载列为废弃，替代接口 asc_set_l0c_copy_lrelu_alpha 仅支持 float；本页原型仍保留，属废弃而非移除。"
---
# asc_set_l0c2gm_lrelu_alpha（废弃）

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

**`asc_set_l0c2gm_lrelu_alpha`接口已废弃。请使用[asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md)接口替代。**

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

用于设置asc_copy_l0c2l1或asc_copy_l0c2gm接口计算过程中使用的Leaky ReLU alpha值。该值只支持half和float两种数据类型。

## 函数原型

```cpp
__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(half& config)
__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(float& config)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| config | 输入 | Leaky ReLU alpha值。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
half config = 0.5;
asc_set_l0c2gm_lrelu_alpha(config);
```
