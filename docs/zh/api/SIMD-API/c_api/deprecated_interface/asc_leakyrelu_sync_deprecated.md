---
okftype: reference.api
description: "已废弃的同步激活接口，按元素执行Leaky ReLU运算并按PIPE_V流水计算，提供half与float两种源类型的重载，替代方案为asc_leakyrelu配合asc_sync。"
tag:
  - roles: [compute]
  - operations: [leaky_relu]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "操作数地址重叠约束见该页通用地址重叠约束节"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "与非同步接口配合完成同步"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_leakyrelu.md
    kind: replaced_by
    condition: "替代本同步激活接口的非同步接口"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "两个源类型重载均带 ASC_DEPRECATED(9.2.0, 2027/09/07, asc_leakyrelu and asc_sync) 标记（include/c_api/composite/vector_compute_composite.h 中对应 @override_id 之前的宏行），替代方案为非同步激活接口 asc_leakyrelu 加 asc_sync；本页全部原型均属废弃范围，属整页废弃，不封可用上限。"
---
# asc_leakyrelu_sync（废弃）

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

**`asc_leakyrelu_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_fused/asc_leakyrelu.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

执行矢量Leaky Relu运算。计算公式如下：

$$
dst_i =
\begin{cases}
src_i ,\quad src_i>0\\
\alpha src_i, \quad src_i\le0&
\end{cases}
$$

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_leakyrelu_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
    __aicore__ inline void asc_leakyrelu_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量）的起始地址。            |
| src | 输入    | 源操作数（矢量）的起始地址，为待处理数据。             |
| value | 输入    | 源操作数（标量），leaky_relu中alpha的值。             |
| count     | 输入    | 参与计算的元素个数。        |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src的起始地址需要32字节对齐。

## 调用示例

```cpp
//total_length指参与计算的数据长度
constexpr uint64_t total_length = 64;
half alpha = 0.1;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_leakyrelu_sync(dst, src, alpha, total_length);
```
