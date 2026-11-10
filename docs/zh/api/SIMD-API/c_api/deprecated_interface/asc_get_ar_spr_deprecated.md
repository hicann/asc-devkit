---
okftype: reference.api
description: "已废弃的特殊寄存器读取接口，无入参并返回 int64_t 类型的特殊寄存器数值，运行在 PIPE_S 且仅在 Ascend 950PR&950DT 支持，建议改用 asc_get_squeeze_status。"
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_squeeze_status.md
    kind: replaced_by
    condition: "替代本接口的squeeze状态读取能力"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include/c_api/spr/spr.h 中声明标记 ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_get_squeeze_status)，CANN 9.2.0 Release Notes 接口废弃表亦列出 asc_get_ar_spr 建议改用 asc_get_squeeze_status；9.3.0-beta.1 声明仍保留，属废弃而非移除。"
---
# asc_get_ar_spr（废弃）

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

头文件路径为：`"c_api/spr/spr.h"`。

**该接口已废弃，请使用[asc_get_squeeze_status](../spr/asc_get_squeeze_status.md)实现此功能。**

读取指定特殊寄存器的值。

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_ar_spr()
```

## 参数说明

无

## 返回值说明

返回int64_t类型的特殊寄存器中的数值。

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

无
