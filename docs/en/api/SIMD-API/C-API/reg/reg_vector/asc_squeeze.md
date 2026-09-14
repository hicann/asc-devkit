# asc_squeeze

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:18:43.359Z pushedAt=2026-09-05T12:14:28.942Z -->

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

Copies the valid elements selected by the mask from **src** to **dst** in sequence. The valid elements are arranged contiguously in **dst**, and the remaining positions in **dst** are set to 0.

When the **asc_squeeze_v2** API is used together with the [asc_storeunalign_postupdate](../reg_store/asc_storeunalign_postupdate.md) API, the valid data size is saved to the AR register, and this value is used as the storage offset when moving data out. The value in the AR register can be obtained through the [asc_get_ar_spr](../../sys_var/asc_get_ar_spr.md) API.

## Prototype

  ```cpp
  __simd_callee__ inline void asc_squeeze(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_half& dst, vector_half src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze(vector_float& dst, vector_float src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_half& dst, vector_half src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_squeeze_v2(vector_float& dst, vector_float src, vector_bool mask)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Destination operand (vector data register). |
| src       | Input    | Source operand (vector data register). |
| mask      | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_int8_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the externally input UB memory space.
vector_int8_t dst;
asc_squeeze(dst, src, mask);
```

```cpp
vector_int8_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the externally input UB memory space.
vector_int8_t dst;
asc_squeeze_v2(dst, src, mask);
```
