---
okftype: reference.api
description: "已废弃的地址寄存器偏移对齐搬入接口，从Unified Buffer连续对齐搬入并通过addr_reg传入偏移，用户可选择更新偏移或源地址，支持三种搬入模式共32个原型，替代方案为语义命名的新接口。"
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
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "offset 必须由该接口族的地址寄存器生成"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:offset"
    scope_basis: resolved
    note: "offset 形参类型由 iter_reg 更名为 addr_reg（同一底层类型 vector_address，语义与取值单位不变）：变更前原型为 asc_loadalign_*_v*(..., iter_reg offset)，变更后为 asc_loadalign_*_v*(..., addr_reg offset)；CANN_9_2_0 Release Notes 第 56 行「Reg搬入、搬出接口的迭代偏移参数由 iter_reg 迁移为 addr_reg」与第 120 行的统一更名条目为据。"
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "本页 32 个原型在 include/c_api/reg_compute/load/loadalign.h 中按模式分别带 ASC_DEPRECATED(9.2.0, 2027/09/07, asc_loadalign_unpack4)、asc_loadalign_brc_datablock、asc_loadalign_brc_elem2datablock 标记；CANN_9_2_0 Release Notes 第 121 行把这些后缀接口列入废弃替换表；废弃不封可用上限。"
---
# asc_loadalign（地址寄存器偏移，废弃）

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

对齐数据搬运接口，从Unified Buffer（UB）连续对齐搬入目的操作数，支持多种搬入模式，接口通过地址寄存器传入偏移，用户可以选择更新偏移或者更新源操作数的地址。

- UNPACKV2搬入模式：解压缩模式，按无符号整型u8加载VL/4长度数据，unpack到VL长度u32类型，中间位置补0。
- BRCV2搬入模式：读取一个DataBlock（32B），并广播到VL。
- BRCV3搬入模式：加载（VL/DataBlock）B的数据，并将每个元素（16bit/32bit）广播到一个DataBlock（32B）中。

## 函数原型

```cpp
// UNPACKV2搬入模式
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset)
// BRCV2搬入模式
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset)
// BRCV3搬入模式
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, addr_reg offset)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数，UB中32B对齐的起始地址。 |
| offset | 输入 | 地址寄存器，必须通过[asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md)接口生成，用于记录相对`src`起始地址的偏移，单位为元素个数。 |

矢量数据寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 该接口中的src需要32B对齐。

## 调用示例

```cpp
__ubuf__ int8_t* src = (__ubuf__ int8_t*)asc_get_phy_buf_addr(0);
addr_reg offset = asc_update_addr_reg_b8(16);
vector_int8_t dst;
asc_loadalign_unpack_v2(dst, src, offset);
```
