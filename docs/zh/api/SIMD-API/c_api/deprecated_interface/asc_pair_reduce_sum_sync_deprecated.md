---
okftype: reference.api
description: "已废弃的相邻元素求和同步接口，把源矢量奇偶相邻的两个元素两两相加写入目的矢量，支持 half 与 float，替代方案为 asc_pair_reduce_sum 配合 asc_sync。"
tag:
  - roles: [compute]
  - operations: [reduction, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_pair_reduce_sum.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "同步复合归约接口废弃，改用 asc_pair_reduce_sum 与 asc_sync；include/c_api/composite/vector_compute_composite.h 的两个声明均带 ASC_DEPRECATED(9.2.0)。"
---
# asc_pair_reduce_sum_sync（废弃）

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

**`asc_pair_reduce_sum_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_reduce/asc_pair_reduce_sum.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

对输入数据做归约操作，得到数据总和。相邻两个（奇偶）元素求和，例如（a1，a2，a3，a4，a5，a6…），相邻两个数据求和为（a1+a2，a3+a4，a5+a6…）。

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
|------------|------------|-----------|
| dst     | 输出     | 目的操作数（矢量）的起始地址。 |
| src     | 输入     | 源操作数（矢量）的起始地址。 |
| count   | 输入     | 参与连续计算的元素个数。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src的起始地址需要32字节对齐。

## 调用示例

```cpp
// 示例计算256个half的相邻元素求和，分2次迭代完成，每次迭代处理128个元素，输出64个结果
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 128;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// 同步接口使用 count 指定参与计算的元素总数，无需手动设置 mask。
asc_pair_reduce_sum_sync(dst, src, src_length);
```

结果示例：

```
输入数据src：[1 2 3 ... 16 17 ... 32 ... 225... 256]
输出数据dst：[3 7 ... 511]
```
