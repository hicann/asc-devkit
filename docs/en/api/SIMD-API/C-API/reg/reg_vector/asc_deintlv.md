# asc_deintlv

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:23:38.398Z pushedAt=2026-09-05T12:14:28.804Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Given source operands **src0** and **src1**, deinterleaves the elements in **src0** and **src1** and stores them into destination operands **dst0** and **dst1**. When the operands are mask registers, the API for different data types determines the interleaving bit width. For example, for the int32_t (b32) type, interleaving is performed in groups of 4 bits.

## Prototype

```cpp
// The operands are vector registers.
__simd_callee__ inline void asc_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1)
__simd_callee__ inline void asc_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1)
__simd_callee__ inline void asc_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, vector_int16_t src0, vector_int16_t src1)
__simd_callee__ inline void asc_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, vector_uint16_t src0, vector_uint16_t src1)
__simd_callee__ inline void asc_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, vector_int8_t src0, vector_int8_t src1)
__simd_callee__ inline void asc_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, vector_uint8_t src0, vector_uint8_t src1)
__simd_callee__ inline void asc_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
__simd_callee__ inline void asc_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, vector_hifloat8_t src0, vector_hifloat8_t src1)
__simd_callee__ inline void asc_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
__simd_callee__ inline void asc_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
__simd_callee__ inline void asc_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, vector_bfloat16_t src0, vector_bfloat16_t src1)
__simd_callee__ inline void asc_deintlv(vector_float& dst0, vector_float& dst1, vector_float src0, vector_float src1)
__simd_callee__ inline void asc_deintlv(vector_half& dst0, vector_half& dst1, vector_half src0, vector_half src1)
// The operands are mask registers.
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0, vector_bool& dst1, vector_bool src0, vector_bool src1)
```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst0 | Output | Destination operand (vector data register/mask register). |
| dst1 | Output | Destination operand (vector data register/mask register). |
| src0 | Input | Source operand (vector data register/mask register). |
| src1 | Input | Source operand (vector data register/mask register). |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_int32_t dst0;
vector_int32_t dst1;
vector_int32_t src0;
vector_int32_t src1;
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); //src1_addr is the externally input UB memory space address.
asc_deintlv(dst0, dst1, src0, src1);
```