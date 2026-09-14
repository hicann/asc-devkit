# asc_store

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-04T11:40:16.984Z pushedAt=2026-09-05T12:14:28.969Z -->

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

A register-based data transfer API, applicable to the scenario of moving data from a vector data register to UB. It does not distinguish whether the data is aligned. When pursuing ultimate performance, it is recommended to use the [asc_storealign_v1](asc_storealign_v1.md), [asc_storealign_v2](asc_storealign_v2.md), [asc_storealign_v3](asc_storealign_v3.md), [asc_storealign_v4](asc_storealign_v4.md), and [asc_storealign_v5](asc_storealign_v5.md) APIs for aligned scenarios.

## Prototype

```cpp
// If the count parameter is not passed, VL (Vector Length) elements are transferred by default.
__simd_callee__ inline void asc_store(__ubuf__  int8_t* dst, vector_int8_t src)
__simd_callee__ inline void asc_store(__ubuf__  uint8_t* dst, vector_uint8_t src)
__simd_callee__ inline void asc_store(__ubuf__  fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src)
__simd_callee__ inline void asc_store(__ubuf__  fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src)
__simd_callee__ inline void asc_store(__ubuf__  int4b_t* dst, vector_int4x2_t src)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e8m0_t* dst, vector_fp8_e8m0_t src)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e5m2_t* dst, vector_fp8_e5m2_t src)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src)
__simd_callee__ inline void asc_store(__ubuf__  hifloat8_t* dst, vector_hifloat8_t src)
__simd_callee__ inline void asc_store(__ubuf__  int16_t* dst, vector_int16_t src)
__simd_callee__ inline void asc_store(__ubuf__  uint16_t* dst, vector_uint16_t src)
__simd_callee__ inline void asc_store(__ubuf__  half* dst, vector_half src)
__simd_callee__ inline void asc_store(__ubuf__  bfloat16_t* dst, vector_bfloat16_t src)
__simd_callee__ inline void asc_store(__ubuf__  int32_t* dst, vector_int32_t src)
__simd_callee__ inline void asc_store(__ubuf__  uint32_t* dst, vector_uint32_t src)
__simd_callee__ inline void asc_store(__ubuf__  float* dst, vector_float src)
__simd_callee__ inline void asc_store(__ubuf__  int64_t* dst, vector_int64_t src)

// If the count parameter is passed, count elements are transferred.
__simd_callee__ inline void asc_store(__ubuf__  int8_t* dst, vector_int8_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  uint8_t* dst, vector_uint8_t src, uint32_t count)  
__simd_callee__ inline void asc_store(__ubuf__  int16_t* dst, vector_int16_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  uint16_t* dst, vector_uint16_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  int32_t* dst, vector_int32_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  uint32_t* dst, vector_uint32_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  float* dst, vector_float src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  int64_t* dst, vector_int64_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  bfloat16_t* dst, vector_bfloat16_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e8m0_t* dst, vector_fp8_e8m0_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e5m2_t* dst, vector_fp8_e5m2_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  fp8_e4m3fn_t* dst, vector_fp8_e4m3fn_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  hifloat8_t* dst, vector_hifloat8_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  fp4x2_e2m1_t* dst, vector_fp4x2_e2m1_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  fp4x2_e1m2_t* dst, vector_fp4x2_e1m2_t src, uint32_t count)
__simd_callee__ inline void asc_store(__ubuf__  int4b_t* dst, vector_int4x2_t src, uint32_t count)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Source operand (vector data register). |
| count | Input | Number of elements to transfer. |

For details about the vector data register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The value range of count is [1, VL/sizeof(dst)], where VL is the vector length. Values outside this range lead to undefined behavior.

## Example

```cpp
vector_half src;
__ubuf__ half* dst = (__ubuf__ half*)asc_get_phy_buf_addr(0);
asc_store(dst, src);
```


