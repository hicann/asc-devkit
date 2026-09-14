# asc_gather_datablock

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:40:40.261Z pushedAt=2026-09-03T12:34:31.465Z -->

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

Given the base address of the source operand in the UB and the indices, gathers DataBlocks from the source operand at the specified indices into the destination operand. Each DataBlock is 32 bytes long.
## Prototype

- Supports mask control, allowing selective gathering of data blocks

    ```cpp
    __simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_half& dst, __ubuf__ half* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_float& dst, __ubuf__ float* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index, vector_bool mask)
    ```
- Without mask control, gathering the data blocks corresponding to all indexes by default
    ```cpp
    __simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_half& dst, __ubuf__ half* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_float& dst, __ubuf__ float* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Start address of the source operand (vector). |
| index | Input | Index position of each DataBlock in dst relative to src in UB. The index position must be greater than or equal to 0 and 32B-aligned. |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of the source operand in UB must be 32B-aligned.
- The index position must be greater than or equal to 0 and 32B-aligned.
- Indexes may contain identical values, meaning that the data of the same DataBlock in the source operand can be read multiple times.

## Example

- Supports mask control, allowing selective gathering of data blocks
    ```cpp
    vector_int8_t dst;
    __ubuf__ int8_t* src = (__ubuf__ int8_t*)asc_get_phy_buf_addr(0);
    vector_uint32_t index;
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    asc_loadalign(index, index_addr); // index_addr is the externally input UB memory space address.
    asc_gather_datablock(dst, src, index, mask);
    ```
- Without mask control, gathering the data blocks corresponding to all indexes by default
    ```cpp
    vector_int8_t dst;
    __ubuf__ int8_t* src = (__ubuf__ int8_t*)asc_get_phy_buf_addr(0);
    vector_uint32_t index;
    asc_loadalign(index, index_addr); // index_addr is the externally input UB memory space address.
    asc_gather_datablock(dst, src, index);
    ```
