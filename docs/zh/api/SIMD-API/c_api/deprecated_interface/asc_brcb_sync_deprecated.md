---
okftype: reference.api
description: "已废弃的矢量广播同步接口，每次取源矢量 8 个元素分别填充到目的的 8 个 32 字节 DataBlock，支持 uint16_t 与 uint32_t 并需逐项传入块步长、迭代步长与迭代次数。"
tag:
  - roles: [compute]
  - operations: [broadcast, synchronization]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_broadcast/asc_brcb.md
    kind: replaced_by
version: ">=8.5.0"
version_changes:
  - since: "9.0.0"
    kind: parameter_changed
    scope: "page"
    scope_basis: kind_default
    note: "参数形态由 8.5.0 的 const asc_brcb_config& config 改为逐项传入 uint16_t dst_block_stride、uint16_t dst_repeat_stride、uint8_t repeat；history_probe 显示 asc_brcb_config& 形式在 8.5.0 之后不再出现，逐项参数声明自 9.0.0-beta.1 起出现在公开头。"
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "同步复合接口废弃，改用 asc_brcb 与 asc_sync；include/c_api/composite/vector_compute_composite.h:143,160 的两个逐项参数声明均带 ASC_DEPRECATED(9.2.0)。"
---
# asc_brcb_sync（废弃）

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

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_brcb_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_broadcast/asc_brcb.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

给定一个输入矢量，每一次取输入矢量中的8个数填充到结果矢量的8个datablock（32Bytes）中去，每个数对应一个datablock。

## 函数原型

- 同步高维切分计算

    ```cpp
    __aicore__ inline void asc_brcb_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_brcb_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| dst_block_stride | 输入 | 目的操作数单次迭代内不同DataBlock间地址步长。 |
| dst_repeat_stride | 输入 | 目的操作数相邻迭代间相同DataBlock的地址步长。 |
| repeat | 输入 | 迭代次数。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 不支持src与dst为同一块内存地址。

## 调用示例

```cpp
constexpr uint32_t src_length = 16;
constexpr uint32_t dst_length = 256;
__ubuf__ uint16_t src[src_length];
__ubuf__ uint16_t dst[dst_length];
asc_brcb_sync(dst, src, 1, 8, 2);
```

结果示例：

```
输入数据src：[1 2 3 ... 16]
输出数据dst：[1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ... 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]
```
