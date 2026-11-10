---
okftype: reference.api
description: "已废弃的融合乘加同步接口，按元素计算src0与src1之积再加到dst上并等待PIPE_V，支持half与float组合，替代方案为asc_fma配合asc_sync。"
tag:
  - roles: [compute]
  - operations: [fused_multiply_add]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "改用非同步接口后需自行调用asc_sync完成同步"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_fma.md
    kind: replaced_by
    condition: "非同步融合乘加接口替代本页同步接口"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include头对half、float×half、float三个重载统一标注ASC_DEPRECATED(9.2.0, 2027/09/07, asc_fma and asc_sync)；CANN_9_2_0 Release Notes废弃表覆盖该类*_sync复合接口；接口保留且仅2201可用。"
---
# asc_fma_sync（废弃）

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

**`asc_fma_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_fused/asc_fma.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

按元素将src0和src1相乘并和dst相加，将最终结果存放进dst中，计算公式如下：
$$
dst_i = (src0_i * src1_i) + dst_i
$$

## 函数原型

- 同步计算

  ```cpp
  __aicore__ inline void asc_fma_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
  ```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量）的起始地址。 |
| src0      | 输入    | 源操作数（矢量）的起始地址。 |
| src1      | 输入    | 源操作数（矢量）的起始地址。 |
| count     | 输入    | 参与计算的元素个数。        |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src0、src1的起始地址需要32字节对齐。

## 调用示例

```cpp
__ubuf__ half dst[256];
__ubuf__ half src0[256];
__ubuf__ half src1[256];
asc_fma_sync(dst, src0, src1, 256);
```
