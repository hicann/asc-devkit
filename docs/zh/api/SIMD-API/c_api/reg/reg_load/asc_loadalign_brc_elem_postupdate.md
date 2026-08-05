# asc_loadalign_brc_elem_postupdate

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

对齐数据搬运接口，从UB连续对齐搬入目的操作数，实现BRC搬入模式并启用Post Update：搬运一个b8/b16/b32类型的数据并Broadcast到所有元素位置，接口调用后自动更新源操作数地址。

## 函数原型

```cpp
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。            |
| src       | 输入/输出 | 源操作数（矢量）的起始地址，接口调用后自动更新。            |
| offset    | 输入    | 偏移量。            |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_half dst;
__ubuf__ half* src;
asc_loadalign_brc_elem_postupdate(dst, src, 0);
```
