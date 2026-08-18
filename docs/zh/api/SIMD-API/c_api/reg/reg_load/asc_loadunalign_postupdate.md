# asc_loadunalign_postupdate

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

数据搬运接口，适用于从Unified Buffer（UB）非32B对齐地址起始连续搬入。

- asc_loadunalign_postupdate（不带addr_reg入参）：配合asc_loadunalign_pre（不带addr_reg入参）接口使用。
- asc_loadunalign_postupdate（带addr_reg入参）：配合asc_loadunalign_pre（带addr_reg入参）接口使用。

## 函数原型

```cpp
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_half& dst, vector_load_unalign& src0, __ubuf__ half*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_float& dst, vector_load_unalign& src0, __ubuf__ float*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_half& dst, vector_load_unalign& src0, __ubuf__ half *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_float& dst, vector_load_unalign& src0, __ubuf__ float *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t *src1, addr_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t *src1, addr_reg& offset, uint32_t inc)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入/输出 | 源操作数（矢量）的起始地址。当使用不含addr_reg参数的接口时，搬运完成后该地址会被自动更新。 |
| offset | 输入/输出 | 存储地址的偏移量。当使用含addr_reg参数的接口时，搬运完成后该偏移量会被自动更新。 |
| count | 输入 | 搬运元素个数。 |
| inc | 输入 | 在多次循环中，多次调用该接口时，每次搬运完成后，下次搬运的offset在自增前会更新为offset + inc。建议设置为0。|

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

- 接口中的src1不需要32B对齐。

## 调用示例

```cpp
vector_int8_t dst;
vector_load_unalign src0;
__ubuf__ int8_t* src_ptr = (__ubuf__ int8_t*)asc_get_phy_buf_addr(0);
uint32_t count = 16;
asc_loadunalign_postupdate(dst, src0, src_ptr, count);
```