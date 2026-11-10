---
okftype: reference.api
description: "已废弃的原子累加配置接口，为后续 UB、L0C 或 L1 到 Global Memory 的搬运开启 int32_t 原子累加，替代方案为语义明确、带数据类型后缀的 asc_set_atomic_add_int32。"
tag:
  - roles: [data_movement_configuration]
  - operations: [add]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "新增 3510（Ascend 950PR&950DT）实现：history_probe 显示 npu_arch_3510/atomic_impl.h 中的 asc_set_atomic_add_int 自 9.1.0 起出现，CANN_9_1_0 增强节列出 asc_set_atomic_add_int；9.0.0 时仅 2201（Atlas A2/A3）可用。"
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "接口废弃，改用带明确数据类型后缀的 asc_set_atomic_add_int32；include/c_api/atomic/datamove_atomic.h:194 起的声明带 ASC_DEPRECATED(9.2.0)，CANN_9_2_0 废弃表同源。"
---
# asc_set_atomic_add_int（废弃）

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

头文件路径为：`"c_api/atomic/datamove_atomic.h"`。

**该接口已废弃，请使用[asc_set_atomic_add_int32](../atomic/datamove_atomic/asc_set_atomic_add.md)替代。**

设置对后续的从Unified Buffer（UB）/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型为int32_t。

## 函数原型

```c
[[deprecated("NOTICE: asc_set_atomic_add_int is deprecated. Please use asc_set_atomic_add_int32 instead.")]]
__aicore__ inline void asc_set_atomic_add_int()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 使用完成后，建议清空原子操作的状态（详见[asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md)），以免影响后续相关指令功能。
- 该指令执行前不会对GM的数据做清零操作，开发者需在需要时手动添加清零操作。

## 调用示例

请参考[asc_set_atomic_add](../atomic/datamove_atomic/asc_set_atomic_add.md)的调用示例。
