---
okftype: reference.api
description: "废弃接口页：3510 架构下 asc_set_store_atomic_config_v2 以 type 与 op 两个 16 位参数设置原子操作启用位与原子操作类型，type 选择 int32、half、float、bfloat16 等数据类型，op 取 2 表示求和，原子加请改用 asc_atomic_add。"
tag:
  - roles: [configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md
    kind: replaced_by
    condition: "Release Notes 9.1.0 要求原子加操作直接调用 asc_atomic_add"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.1.0 废弃表把 asc_set_store_atomic_config_v2 列为废弃并要求标量原子加直接调用 asc_atomic_add，页面原型带有 deprecated 属性；这是废弃而非移除，接口仍然可用。"
---
# asc_set_store_atomic_config_v2（废弃）

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

头文件路径为：`"c_api/atomic/scalar_atomic.h"`。

**该接口已废弃。原子加操作请使用[asc_atomic_add](../atomic/scalar_atomic/asc_atomic_add.md)实现。**

设置原子操作启用位与原子操作类型的值。

## 函数原型

```c
[[deprecated("NOTICE: asc_set_store_atomic_config_v2 is deprecated."
             "Please use asc_atomic_add instead for atomic add operation.")]]
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| type | 输入 | 原子操作启用位。<br>1：开启原子操作，进行原子操作的数据类型为int32_t。<br>4：开启原子操作，进行原子操作的数据类型为half。<br>5：开启原子操作，进行原子操作的数据类型为float。<br>7：开启原子操作，进行原子操作的数据类型为bfloat16_t。<br>9：开启原子操作，进行原子操作的数据类型为int16_t。<br>10：开启原子操作，进行原子操作的数据类型为int8_t。<br>其余值无具体含义。 |
| op | 输入 | 原子操作类型。<br>2：求和操作。<br>其余值无具体含义。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```c
uint16_t type = 1;
uint16_t op = 2;
asc_set_store_atomic_config_v2(type, op);
```
