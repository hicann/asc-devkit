# asc_storeunalign_postupdate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:50:36.441Z pushedAt=2026-09-05T12:14:28.977Z -->

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

Header file path: `"c_api/reg_compute/reg_store.h"`.

A reg compute data transfer API, applicable to the scenario of continuously moving data out to the UB from a vector data register or mask register starting at a non-32B-aligned address.

Each call to this API updates the address of the destination operand on the UB.

## Prototype

- Storage offset not specified

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_bool src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_bool src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_bool src1)
    ```

- uint32_t as the storage offset

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t*& dst, vector_store_unalign& src0, vector_int8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_uint8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t*& dst, vector_store_unalign& src0, vector_int16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t*& dst, vector_store_unalign& src0, vector_uint16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t*& dst, vector_store_unalign& src0, vector_int32_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t*& dst, vector_store_unalign& src0, vector_uint32_t src1, uint32_t count) 
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t*& dst, vector_store_unalign& src0, vector_int64_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half*& dst, vector_store_unalign& src0, vector_half src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float*& dst, vector_store_unalign& src0, vector_float src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t*& dst, vector_store_unalign& src0, vector_bfloat16_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t*& dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ hifloat8_t*& dst, vector_store_unalign& src0, vector_hifloat8_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t*& dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t*& dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t*& dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t*& dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, uint32_t count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t*& dst, vector_store_unalign& src0, vector_int4x2_t src1, uint32_t count)
    ```

- iter_reg as the storage offset

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ hifloat8_t* dst, vector_store_unalign& src0, vector_hifloat8_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, iter_reg& count)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1, iter_reg& count)
    ```

## Parameters

- Storage offset not specified

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src0 | Input/Output | Unaligned register used to save unaligned data, with a length of 32B. |
    | src1 | Input | Source operand (vector data register or mask register). |

- uint32_t as the storage offset

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src0 | Input/Output | Unaligned register used to save unaligned data, with a length of 32B. |
    | src1 | Input | Source operand (vector data register). |
    | count | Input | Number of storage transfers. |

- iter_reg as the storage offset.

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src0 | Input/Output | Unaligned register used to save unaligned data, with a length of 32B. |
    | src1 | Input | Source operand (vector data register). |
    | count | Input | Number of storage transfers (address register). |

For details about vector data registers, unaligned registers, and address registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- **dst** in this API does not need to be 32B aligned.
- When **uint32_t** is used as the storage offset, this API must be used together with [asc_storeunalign_post_postupdate](./asc_storeunalign_post_postupdate.md). The **asc_storeunalign_post_postupdate** API is used to process the tail block of the unaligned transfer in this scenario.
- When **iter_reg** is used as the storage offset, this API must be used together with [asc_storeunalign_post](./asc_storeunalign_post.md). The **asc_storeunalign_post** API is used to process the tail block of the unaligned transfer in this scenario.

## Example

- Unspecified storage offset.

    ```cpp
    // dst is the UB address, with a start address of 8, which is not 32B aligned.
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```

- Continuous unaligned move-out from the mask register

    ```cpp
    // dst is the UB address, with a start address of 8, which is not 32B-aligned.
    vector_store_unalign ureg;
    vector_bool src;
    uint32_t remain_count = 128;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // Prepare the src to be moved out this time.
        src = asc_update_src_b16(remain_count);
        // The tail block crossing the 32B boundary is buffered in ureg.
        asc_storeunalign_postupdate(dst, ureg, src);
    }
    // After the loop ends, call post once to flush the remaining data in ureg to UB; dst has auto-incremented to the end position, and offset is 0.
    asc_storeunalign_post(dst, ureg, 0);
    ```

- uint32_t as the storage offset

    ```cpp
    // dst is the UB address, with a start address of 8, which is not 32B-aligned.
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t count = 64;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // Other reg operations.

        // First time: src[0:62] is written to dst[0:62] (that is, UB[8:256]); meanwhile src[62:64] is written to ureg[0:2]; meanwhile dst auto-increments to dst[64] (that is, UB[264]).
        // Second time: ureg[0:2] + src[0:62] is written to dst[-2:62] (that is, UB[256:512]); meanwhile src[62:64] is written to ureg[0:2]; meanwhile dst auto-increments to dst[128] (that is, UB[520]).
        asc_storeunalign_postupdate(dst, ureg, src, count);
    }
    // Write ureg[0:2] to dst[-2:0] (that is, UB[512:520]); dst is not incremented.
    asc_storeunalign_post_postupdate(dst, ureg, 0);
    ```

- iter_reg as the storage offset.

    ```cpp
    // dst is the UB address, with a start address of 8, which is not 32B-aligned.
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        iter_reg count = asc_create_iter_reg_b32(64);
        // Other reg operations.

        // First time: Write src[0:62] to dst[0:62] (that is, UB[8:256]); write src[62:64] to ureg[0:2]; and increment dst to dst[64] (that is, UB[264]).
        // Second time: Write ureg[0:2] + src[0:62] to dst[-2:62] (that is, UB[256:512]); write src[62:64] to ureg[0:2]; and increment dst to dst[128] (that is, UB[520]).
        asc_storeunalign_postupdate(dst, ureg, src, count);
    }
    // The end address of dst is 520, but dst has already been incremented to 520, so offset must be configured to 0.
    iter_reg offset = asc_create_iter_reg_b32(0);
    // Write ureg[0:2] to dst[-2:0] (that is, UB[512:520]).
    asc_storeunalign_post(dst, ureg, offset);
    ```
