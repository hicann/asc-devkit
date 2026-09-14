# asc_storealign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-05T12:00:28.632Z pushedAt=2026-09-05T12:14:48.514Z -->

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

A reg computation data transfer API, applicable to the scenario of continuous aligned move-out from a vector data register or mask register to the UB. It supports multiple move-out modes. The API passes in the offset through int32_t and simultaneously configures hardware automatic Post Update, so users do not need to update the offset or the address of the destination operand.

- Scenarios where data is moved out from a vector data register to the UB with continuous alignment
  - NORM move-out mode: normal mode, which moves VL data. Supported data types are b8, b16, and b32.
  - FIRST move-out mode: ignores the mask and moves the first element of src to dst_align32b. Supported data types are b8, b16, and b32.
  - PACK move-out mode: compression mode, which continuously moves the low-half bit data of the valid elements in src to dst_align32b based on the mask. Supported data types are b16, b32, and b64.
  - PACKV2 move-out mode: compression mode, which continuously moves the low 8 bits of the valid elements in src to dst_align32b based on the mask. The supported data type is b32.
- Scenarios where data is moved out from a mask register to the UB with continuous alignment
  - NORM move-out mode: normal mode, which moves VL data. Supported data types are u8, u16, and u32.
  - PACK move-out mode: compression mode, in which the lower half of the bit data of the elements in src is stored contiguously in dst. The supported data types are u8, u16, and u32.

## Prototype

- Scenarios where data is moved out from a vector data register to the UB with continuous alignment
  ```cpp
  // NORM move-out mode.
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t*& dst_align32b, vector_int8_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst_align32b, vector_uint8_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int16_t*& dst_align32b, vector_int16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint16_t*& dst_align32b, vector_uint16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ half*& dst_align32b, vector_half src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ bfloat16_t*& dst_align32b, vector_bfloat16_t src, int32_t offset, vector_bool mask)
  
  // FIRST move-out mode.
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  int8_t*& dst_align32b, vector_int8_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  uint8_t*& dst_align32b, vector_uint8_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  int16_t*& dst_align32b, vector_int16_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  uint16_t*& dst_align32b, vector_uint16_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  int32_t*& dst_align32b, vector_int32_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  half*& dst_align32b, vector_half src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  float*& dst_align32b, vector_float src, int32_t offset)
  __simd_callee__ inline void asc_storealign_1st_postupdate(__ubuf__  bfloat16_t*& dst_align32b, vector_bfloat16_t src, int32_t offset)
  
  // PACK move-out mode.
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  int16_t*& dst_align32b, vector_int16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  uint16_t*& dst_align32b, vector_uint16_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  int64_t*& dst_align32b, vector_int64_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  uint64_t*& dst_align32b, vector_uint64_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  half*& dst_align32b, vector_half src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__  bfloat16_t*& dst_align32b, vector_bfloat16_t src, int32_t offset, vector_bool mask)

  // PACKV2 move-out mode.
  __simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__  int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__  uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
  __simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__  float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
  ```
- Scenarios where data is moved out from a mask register to the UB with continuous alignment
  ```cpp
  // NORM move-out mode.
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset)
  
  // PACK move-out mode.
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint32_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint16_t*& dst, vector_bool src, int32_t offset)
  __simd_callee__ inline void asc_storealign_pack_postupdate(__ubuf__ uint8_t*& dst, vector_bool src, int32_t offset)
  ```

## Parameters

- Scenarios where data is moved out from a vector data register to the UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | dst_align32b | Input/Output | Start address of the destination operand (vector). After the transfer is complete, this address is automatically Post Updated by the hardware. |
  | src | Input | Source operand (vector data register). |
  | mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. |
  | offset | Input    | Address offset.       |

- Scenarios where data is moved out from a mask register to the UB with continuous alignment

  | Parameter  | Input/Output | Description |
  | :----- | :------- | :------- |
  | dst | Input/Output | Start address of the destination operand (vector). After the transfer is complete, this address is automatically Post Updated by the hardware. |
  | src | Input | Source operand (mask register). |
  | offset | Input    | Address offset.       |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

- Scenarios where data is moved out from a vector data register to the UB with continuous alignment
  ```cpp
  __ubuf__ half* dst_align32b = (__ubuf__ half*)asc_get_phy_buf_addr(0);
  vector_half src;
  vector_bool mask = asc_create_mask_b16(PAT_ALL);
  int32_t offset = 128;
  asc_storealign_postupdate(dst_align32b, src, offset, mask);
  ```
- Scenarios where data is moved out from a mask register to the UB with continuous alignment
  ```cpp
  __ubuf__ uint32_t*& dst = (__ubuf__ uint32_t*&)asc_get_phy_buf_addr(0);
  vector_bool src;
  int32_t offset = 0;
  asc_storealign_postupdate(dst, src, offset);
  ```
