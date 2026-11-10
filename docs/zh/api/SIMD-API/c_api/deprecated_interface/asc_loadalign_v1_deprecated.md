---
okftype: reference.api
description: "已废弃的无偏移对齐搬入接口，从Unified Buffer对齐搬入并按UNPACKV2、BRCV2、BRCV3三种模式处理数据，需由用户自行更新源地址，共32个按数据类型区分的原型，替代方案为语义命名的新接口。"
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "矢量数据寄存器与地址寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md
    kind: replaced_by
    condition: "替代BRCV2搬入模式"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md
    kind: replaced_by
    condition: "替代BRCV3搬入模式"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md
    kind: replaced_by
    condition: "替代UNPACKV2搬入模式"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "本页 32 个原型在 include/c_api/reg_compute/load/loadalign.h 中按模式分别带 ASC_DEPRECATED(9.2.0, 2027/09/07, asc_loadalign_unpack4)、ASC_DEPRECATED(9.2.0, 2027/09/07, asc_loadalign_brc_datablock)、ASC_DEPRECATED(9.2.0, 2027/09/07, asc_loadalign_brc_elem2datablock) 标记；CANN_9_2_0 Release Notes 第 121 行把 asc_loadalign_brc_v2、brc_v3、unpack_v2 列入废弃替换表；废弃不封可用上限。"
---
# asc_loadalign（无偏移，废弃）

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

头文件路径为：`"c_api/reg_compute/load/loadalign.h"`。

**该接口已废弃。其中asc_loadalign_brc_v2请使用[asc_loadalign_brc_datablock](../reg_compute/load/asc_loadalign_brc_datablock.md)，asc_loadalign_brc_v3请使用[asc_loadalign_brc_elem2datablock](../reg_compute/load/asc_loadalign_brc_elem2datablock.md)，asc_loadalign_unpack_v2请使用[asc_loadalign_unpack4](../reg_compute/load/asc_loadalign_unpack4.md)实现此功能。**

对齐数据搬运接口，从Unified Buffer（UB）连续对齐搬入目的操作数，支持多种搬入模式，接口不传入偏移，需要由用户自行更新源操作数的地址。

- UNPACKV2搬入模式：解压缩模式，按无符号整型u8加载VL/4长度数据，unpack到VL长度u32类型，中间位置补0。
- BRCV2搬入模式：读取一个DataBlock（32B），并广播到VL。
- BRCV3搬入模式：加载（VL/DataBlock）B的数据，并将每个元素（16bit/32bit）广播到一个DataBlock（32B）中。

## 函数原型

```cpp
// UNPACKV2搬入模式
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src)
// BRCV2搬入模式
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src)
// BRCV3搬入模式
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。            |
| src | 输入    | 源操作数（矢量）的起始地址。            |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_int8_t dst;
__ubuf__ int8_t* src;
asc_loadalign_unpack_v2(dst, src);
```
