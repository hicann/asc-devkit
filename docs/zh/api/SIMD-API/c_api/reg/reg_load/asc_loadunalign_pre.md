# asc_loadunalign_pre

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

用于在进行非对齐数据搬入前的初始化，需配合[asc_loadunalign](asc_loadunalign.md)接口使用。

- asc_loadunalign_pre（不带addr_reg入参）：配合asc_loadunalign（不带addr_reg入参）接口或asc_loadunalign_postupdate（不带addr_reg入参）使用。

- asc_loadunalign_pre（带addr_reg入参）：配合asc_loadunalign_postupdate（带addr_reg入参）接口使用。

## 函数原型

```cpp
// 不带addr_reg入参
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src)
// 带addr_reg入参
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src, addr_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src, addr_reg offset)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 非对齐寄存器，用于保存非对齐数据，长度32B。            |
| src | 输入    | 源操作数（矢量）的起始地址。            |
| offset | 输入    | 地址寄存器，存储地址偏移量。       |

非对齐寄存器的详细说明请参见[reg数据类型定义](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_load_unalign dst;
__ubuf__ int8_t* src;
asc_loadunalign_pre(dst, src);
```
