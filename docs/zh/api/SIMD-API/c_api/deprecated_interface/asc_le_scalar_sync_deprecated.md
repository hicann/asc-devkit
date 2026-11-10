---
okftype: reference.api
description: "已废弃的同步比较接口，按元素判断src是否小于等于标量value并把每个元素的比较结果压成1个bit写入目的地址，提供half与float两种源类型重载，替代方案为asc_le_scalar配合asc_sync。"
tag:
  - roles: [compute]
  - operations: [comparison]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "操作数地址重叠约束见该页通用地址重叠约束节"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "与非同步接口配合完成同步"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_le_scalar.md
    kind: replaced_by
    condition: "替代本同步接口的非同步比较接口"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "两个源类型重载均带 ASC_DEPRECATED(9.2.0, 2027/09/07, asc_le_scalar and asc_sync) 标记（include/c_api/composite/vector_compute_composite.h 中两个 @override_id 之前的宏行），替代方案为非同步比较接口 asc_le_scalar 加 asc_sync；本页全部原型均属废弃范围，属整页废弃，不封可用上限。"
---
# asc_le_scalar_sync（废弃）

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

**`asc_le_scalar_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_compare/asc_le_scalar.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

按元素判断src <= value是否成立，若成立则输出结果为1，否则为0。每个元素的比较结果占1个bit。

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_le_scalar_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_le_scalar_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| ------------ | ------------ | ------------ |
|dst|输出|目的操作数（矢量）的起始地址。|
|src|输入|源操作数（矢量）的起始地址。|
|value|输入|源操作数（标量）。|
| repeat | 输入 | 迭代次数。 |
| dst_block_stride | 输入 | 目的操作数单次迭代内不同DataBlock间地址步长。 |
| src_block_stride | 输入 | 源操作数单次迭代内不同DataBlock间地址步长。 |
| dst_repeat_stride | 输入 | 目的操作数相邻迭代间相同DataBlock的地址步长。 |
| src_repeat_stride | 输入 | 源操作数相邻迭代间相同DataBlock的地址步长。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src的起始地址需要32字节对齐。
- dst按照小端顺序排序成二进制结果，对应src中相应位置的数据比较结果。
- 当结果输出到目的地址中时，mask参数不生效。

## 调用示例

```cpp
// 结果输出到目标地址中，固定为128个元素
constexpr uint32_t total_length = 128;
__ubuf__ uint8_t dst[total_length / 8];
__ubuf__ half src[total_length];
half scalar = 20;
uint8_t repeat = 1;
uint8_t dst_block_stride = 1;
uint8_t src_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src_repeat_stride = 8;
…… // 数据搬运及同步操作
asc_le_scalar_sync(dst, src, scalar, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
…… // 同步操作
```
