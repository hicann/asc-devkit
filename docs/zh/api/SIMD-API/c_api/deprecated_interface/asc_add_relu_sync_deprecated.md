---
okftype: reference.api
description: "已废弃的矢量加法加 ReLU 同步接口，先对两个源矢量逐元素求和再与 0 比较取较大值，并支持目的类型转换组合，仅在 Atlas A2、A3 支持，替代方案为 asc_add_relu 配合 asc_sync。"
tag:
  - roles: [compute]
  - operations: [add, relu, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_add_relu.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "复合同步接口族整体废弃，改用 asc_add_relu 与 asc_sync 分步调用；include/c_api/composite/vector_compute_composite.h 中 6 个声明均带 ASC_DEPRECATED(9.2.0)。"
---
# asc_add_relu_sync（废弃）

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

**`asc_add_relu_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_fused/asc_add_relu.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

按元素求和，再进行Relu计算（结果和0对比取较大值），并提供转换最终结果的数据类型的功能(s162s8、f322f16、f162s8)。计算公式如下：

$$
dst_i = relu(src0_i + src1_i)
$$

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                                |
|:----------| :-----|:----------------------------------|
| dst       | 输出 | 目的操作数（矢量）的起始地址。                   |
| src0、src1 | 输入 | 源操作数（矢量）的起始地址。                        |
| count     | 输入 | 参与计算的元素个数。                        |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- dst、src0、src1的起始地址需要32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。

## 调用示例

```cpp
// 目的操作数和源操作数的数据类型一致
// total_length指参与计算的数据总长度
constexpr uint64_t total_length = 128;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];
asc_add_relu_sync(dst, src0, src1, total_length);
```

```cpp
// 目的操作数和源操作数的数据类型不一致（s162s8）
// total_length指参与计算的数据长度
constexpr uint32_t total_length = 32;
__ubuf__ int8_t dst[total_length];
__ubuf__ int16_t src0[total_length];
__ubuf__ int16_t src1[total_length];
asc_add_relu_sync(dst, src0, src1, total_length);
```
