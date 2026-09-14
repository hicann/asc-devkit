# asc_storealign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-04T11:42:15.502Z pushedAt=2026-09-05T12:14:28.971Z -->

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

This is a register-based data transfer API. It is applicable to scenarios where data is moved out from a vector data register or a mask register to the UB with continuous alignment, and it supports multiple move-out modes. The API passes in the offset through an address register, and the user can choose to update the offset or update the address of the destination operand.

- Scenarios where data is moved out from a vector data register to the UB with continuous alignment
  - NORM move-out mode: normal mode, which moves VL data. Supported data types are b8, b16, and b32.
  - FIRST move-out mode: ignores the mask and moves the first element of src to dst_align32b. Supported data types are b8, b16, and b32.
  - PACK move-out mode: compression mode, which continuously stores the low-half bit data of the valid elements in src to dst_align32b based on the mask. Supported data types are b16, b32, and b64.
  - PACKV2 move-out mode: compression mode, which continuously stores the low 8 bits of the valid elements in src to dst_align32b based on the mask. The supported data type is b32.
  - INTLV move-out mode: dual move-out mode, which ignores the mask and interleaves the elements in src0 and src1 into dst, with a storage length of VL*2.
- Scenarios where data is moved out from a mask register to the UB with continuous alignment
  - NORM move-out mode: normal mode, which moves VL data. Supported data types are u8, u16, and u32.
  - PACK move-out mode: compression mode, in which the lower half of the bit data of the elements in src is stored contiguously in dst. The supported data types are u8, u16, and u32.

## Prototype

- Scenario of moving data out from a vector data register to the UB with continuous alignment
  ```cpp
  // NORM move-out mode.
  __simd_callee__ inline void asc_storealign(__ubuf__  int8_t* dst_align32b, vector_int8_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int16_t* dst_align32b, vector_int16_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int32_t* dst_align32b, vector_int32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  half* dst_align32b, vector_half src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  float* dst_align32b, vector_float src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src, iter_reg offset, vector_bool mask)

  // FIRST move-out mode.
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int8_t* dst_align32b, vector_int8_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int16_t* dst_align32b, vector_int16_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int32_t* dst_align32b, vector_int32_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  half* dst_align32b, vector_half src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  float* dst_align32b, vector_float src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src, iter_reg offset)
  __simd_callee__ inline void asc_storealign_1st(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src, iter_reg offset)

  // PACK move-out mode.
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int16_t* dst_align32b, vector_int16_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int32_t* dst_align32b, vector_int32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  int64_t* dst_align32b, vector_int64_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  uint64_t* dst_align32b, vector_uint64_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  half* dst_align32b, vector_half src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  float* dst_align32b, vector_float src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src, iter_reg offset, vector_bool mask)
  
  // PACKV2 move-out mode.
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  int32_t* dst_align32b, vector_int32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src, iter_reg offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_v2(__ubuf__  float* dst_align32b, vector_float src, iter_reg offset, vector_bool mask)
  
  // INTLV move-out mode.
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int8_t* dst_align32b, vector_int8_t src0, vector_int8_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint8_t* dst_align32b, vector_uint8_t src0, vector_uint8_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int16_t* dst_align32b, vector_int16_t src0, vector_int16_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint16_t* dst_align32b, vector_uint16_t src0, vector_uint16_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int32_t* dst_align32b, vector_int32_t src0, vector_int32_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  uint32_t* dst_align32b, vector_uint32_t src0, vector_uint32_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  half* dst_align32b, vector_half src0, vector_half src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  bfloat16_t* dst_align32b, vector_bfloat16_t src0, vector_bfloat16_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src0, vector_fp8_e4m3fn_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  hifloat8_t* dst_align32b, vector_hifloat8_t src0, vector_hifloat8_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src0, vector_fp8_e5m2_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src0, vector_fp8_e8m0_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp4x2_e2m1_t* dst_align32b, vector_fp4x2_e2m1_t src0, vector_fp4x2_e2m1_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  fp4x2_e1m2_t* dst_align32b, vector_fp4x2_e1m2_t src0, vector_fp4x2_e1m2_t src1, iter_reg offset)
  __simd_callee__ inline void asc_storealign_intlv(__ubuf__  int4b_t* dst_align32b, vector_int4x2_t src0, vector_int4x2_t src1, iter_reg offset)
  ```
- Scenario of moving data out from a mask register to the UB with continuous alignment
  ```cpp
  // NORM move-out mode.
  __simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst, vector_bool src, iter_reg offset)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst, vector_bool src, iter_reg offset)
  __simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst, vector_bool src, iter_reg offset)
  
  // PACK move-out mode.
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint32_t* dst,  vector_bool src,  iter_reg offset)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint16_t* dst,  vector_bool src,  iter_reg offset)
  __simd_callee__ inline void asc_storealign_pack(__ubuf__ uint8_t* dst,  vector_bool src,  iter_reg offset)
  ```

## Parameters

- Scenario of moving data out from a vector data register to the UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | **dst_align32b** | Output | Start address of the destination operand (vector). |
  | **src** | Input | Source operand (vector data register). |
  | **src0** | Input | Source operand 0 (vector data register). |
  | **src1** | Input | Source operand 1 (vector data register). |
  | **mask** | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |
  | **offset** | Input | Address register that stores the address offset. |

- Scenario of moving data out from a mask register to the UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | **dst** | Output | Start address of the destination operand (vector). |
  | **src** | Input | Source operand (mask register). |
  | **offset** | Input | Address register that stores the address offset. |

For details about vector data registers, mask registers, and address registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

- Scenario of moving data out from a vector data register to the UB with continuous alignment
  ```cpp
  __ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
  vector_half src;
  vector_bool mask = asc_create_mask_b16(PAT_ALL);
  uint32_t offset = 128;
  iter_reg addr_reg = asc_create_iter_reg_b16(offset);
  asc_storealign(dst_align32b, src, addr_reg, mask);
  ```
- Scenario of moving data out from a mask register to the UB with continuous alignment
  ```cpp
  __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(0);
  vector_bool src;
  uint32_t offset = 0;
  iter_reg addr_reg = asc_create_iter_reg_b16(offset);
  asc_storealign(dst, src, addr_reg);
  ```
