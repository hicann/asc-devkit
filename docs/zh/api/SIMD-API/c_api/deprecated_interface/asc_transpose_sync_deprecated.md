---
okftype: reference.api
description: "废弃接口页：asc_transpose_sync 对 16x16 的二维矩阵数据块做转置，提供 int16_t 与 uint16_t 两种形式，dst 与 src 需 32 字节对齐，建议改用非同步 asc_transpose 加 asc_sync。"
tag:
  - roles: [compute]
  - operations: [transpose]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "地址重叠约束按其通用地址重叠约束"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "替代方案中承担同步等待"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_transpose.md
    kind: replaced_by
    condition: "建议改用对应的非同步接口并显式调用 asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include 头在 asc_transpose_sync 声明前标注 ASC_DEPRECATED(9.2.0)，Release Notes 9.2.0 把 _sync 复合接口整体列为废弃；接口未移除。"
---
# asc_transpose_sync（废弃）

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

头文件路径为：`"c_api/composite/vector_compute_composite.h"`。

**`asc_transpose_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_permute_sel/asc_transpose.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

用于实现16*16的二维矩阵数据块转置。

## 函数原型

- 同步计算

  ```cpp
    __aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
    __aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
  ```

## 参数说明

**表1** 参数说明

|参数名|输入/输出|描述|
| ------------ | ------------ | ------------ |
|dst|输出|目的操作数（矢量）的起始地址。|
|src|输入|源操作数（矢量）的起始地址。|

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。
- dst、src的起始地址需要32字节对齐。

## 调用示例

```cpp
// total_length指参与计算的数据总长度
constexpr int total_length = 256;
__ubuf__ int16_t dst[total_length];
__ubuf__ int16_t src[total_length];
// dst指目的操作数的地址
asc_transpose_sync(dst, src);
```
