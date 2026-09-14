# asc_storealign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-04T11:42:11.449Z pushedAt=2026-09-05T12:14:28.970Z -->

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

A register-based data movement API, applicable to scenarios where data is moved out from a **vector data** register or **mask register** to UB with **continuous alignment**. It supports multiple **move-out modes**. The API does not take an offset; the user must update the destination operand address.

- Scenarios of moving out from a **vector data** register to UB with **continuous alignment**
  - **NORM** move-out mode: normal mode, which moves **VL** data. Supported data types are **b8**, **b16**, and **b32**.
  - **FIRST** move-out mode: ignores **mask** and moves the first element of **src** to **dst_align32b**. Supported data types are **b8**, **b16**, and **b32**.
  - **PACK** move-out mode: compression mode, which continuously stores the low-half bit data of valid elements in **src** to **dst_align32b** based on **mask**. Supported data types are **b16**, **b32**, and **b64**.
  - **PACKV2** move-out mode: compression mode, which continuously stores the low 8-bit data of valid elements in **src** to **dst_align32b** based on **mask**. Supported data type is **b32**.
  - **INTLV** move-out mode: dual move-out mode, which ignores **mask** and interleaves the elements of **src0** and **src1** into **dst**, with a storage length of **VL** * 2.
- Scenarios of moving out from a **mask register** to UB with **continuous alignment**
    - **NORM** move-out mode: normal mode, which moves **VL** data. Supported data types are **u8**, **u16**, and **u32**.
    - PACK move-out mode: compression mode, in which the lower half of the bit data of the elements in src is stored contiguously in dst. Supported data types are u8, u16, and u32.

## Prototype

- Scenario of moving out from the vector data register to UB with continuous alignment
  ```cpp
  // NORM move-out mode
  __simd_callee__ inline void asc_storealign(__ubuf__  int8_t* dst_align32b, vector_int8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int16_t* dst_align32b, vector_int16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  half* dst_align32b, vector_half src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  float* dst_align32b, vector_float src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src, vector_bool mask)

  // FIRST move-out mode
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int8_t* dst_align32b, vector_int8_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int16_t* dst_align32b, vector_int16_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int32_t* dst_align32b, vector_int32_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  half* dst_align32b, vector_half src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  float* dst_align32b, vector_float src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src)

  // PACK move-out mode
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int16_t* dst_align32b, vector_int16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int64_t* dst_align32b, vector_int64_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint64_t* dst_align32b, vector_uint64_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  half* dst_align32b, vector_half src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  float* dst_align32b, vector_float src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src, vector_bool mask)
  
  // PACKV2 move-out mode
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  float* dst_align32b, vector_float src, vector_bool mask)
  
  // INTLV move-out mode
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int8_t* dst_align32b, vector_int8_t src0, vector_int8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src0, vector_uint8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int16_t* dst_align32b, vector_int16_t src0, vector_int16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src0, vector_uint16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int32_t* dst_align32b, vector_int32_t src0, vector_int32_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src0, vector_uint32_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  half* dst_align32b, vector_half src0, vector_half src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src0, vector_bfloat16_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src0, vector_hifloat8_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src0, vector_int4x2_t src1)
  ```
- Scenario of moving out from the mask register to UB with continuous alignment
  ```cpp
  // NORM move-out mode
  __simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src)
  
  // PACK move-out mode
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst, vector_bool src)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst, vector_bool src)
  ```
## Parameters

- Scenario of moving out from the vector data register to UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | dst_align32b | Output | Start address of the destination operand (vector). |
  | src | Input | Source operand (vector data register). |
  | src0 | Input | Source operand 0 (vector data register). |
  | src1 | Input | Source operand 1 (vector data register). |
  | mask | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. A value of 1 at the corresponding position indicates participation in the computation, and 0 indicates no participation. |

- Scenario of moving out from the mask register to UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | dst | Output | Start address of the destination operand (vector). |
  | src | Input | Source operand (mask register). |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

- Scenario of moving out from the vector data register to UB with continuous alignment
  ```cpp
  __ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
  vector_half src;
  vector_bool mask = asc_create_mask_b16(PAT_ALL);
  asc_storealign(dst_align32b, src, mask);
  ```
- Scenario of moving out from the mask register to UB with continuous alignment
  ```cpp
  __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(0);
  vector_bool src;
  asc_storealign(dst, src);
  ```
