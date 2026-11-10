---
okftype: reference.api
description: "已废弃的int16_t反量化同步接口，按asc_set_deq_scale配置的scale与offset把int16_t转为int8_t或uint8_t，并存入每个DataBlock的高半块或低半块，替代方案为对应的非同步接口配合asc_sync。"
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [datablock_granularity, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "改用非同步接口后需自行调用asc_sync完成同步"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int162b8.md
    kind: replaced_by
    condition: "h与l两种非同步接口替代本页的同步接口"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: requires
    condition: "调用前需先设置量化scale与offset"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include头对h与l两条同步接口的全部4个重载标注ASC_DEPRECATED(9.2.0, 2027/09/07, asc_deq_int162b8_h/l and asc_sync)；CANN_9_2_0 Release Notes废弃表列出该类*_sync复合接口；废弃不封闭可用范围，仍支持Atlas A2与Atlas A3系列产品。"
---
# asc_deq_int162b8_sync（废弃）

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

**`asc_deq_int162b8_h_sync`和`asc_deq_int162b8_l_sync`接口已废弃，请使用[对应的非同步接口](../vector_compute/vector_fused/asc_deq_int162b8.md)和同步接口[asc_sync](../sync/intra_core_sync/asc_sync.md)替代。**

将int16_t类型转换为int8_t或uint8_t类型，并将数据存放在每个DataBlock的高半块或低半块。使用该接口前需要调用[asc_set_deq_scale](../vector_compute/vector_fused/asc_set_deq_scale.md)接口设置量化参数。

- asc_deq_int162b8_h：将数据存放在每个DataBlock的高半块。
- asc_deq_int162b8_l：将数据存放在每个DataBlock的低半块。

如下图所示：
![](../../../figures/asc_deq_int162b8_halfblock.png)

计算公式如下：

$$
dst_i = (src_i * scale) + offset
$$

## 函数原型

- 同步计算

    ```cpp
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）的起始地址。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| count | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- dst、src的起始地址需要32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../general_description_and_constraints.md#通用地址重叠约束)。

## 调用示例

```cpp
constexpr uint64_t total_length = 128;    // total_length指参与计算的数据长度
__ubuf__ int16_t src[total_length];
__ubuf__ int8_t dst[total_length];
float scale = 1.0;        // 量化参数为1
int16_t offset = 0;       // 不带偏移
bool sign_mode = true;    // 量化结果带符号（dst为int8_t类型）
asc_set_deq_scale(scale, offset, sign_mode);    // 计算公式为dst = src
asc_deq_int162b8_h_sync(dst, src, total_length);    // 将src转换为int8_t类型并存放在dst每个DataBlock的高半块
```
