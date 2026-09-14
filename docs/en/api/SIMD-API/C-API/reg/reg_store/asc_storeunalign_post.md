# asc_storeunalign_post

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:48:32.298Z pushedAt=2026-09-05T12:14:28.978Z -->

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

A reg compute data transfer API, applicable to the tail block scenario where data is continuously moved out from a non-32B-aligned start address of a vector data register to UB.

Before calling this API, one of the following APIs must be called first.
- The [asc_storeunalign](./asc_storeunalign.md) API.
- The [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md) API.

The transfer principle is as follows:
Let the start address of the destination operand be dst_start, the end address be dst_end, and the number of tail block elements be unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T).
The transferred data is then divided into two parts:
- The 32B-aligned main block part UB[dst_start, dst_end - unalign_count * sizeof(T)].
- The non-32B-aligned tail block part UB[dst_end - unalign_count * sizeof(T), dst_end].
When the **store_unalign** or **store_unalign_postupdate** API is executed, the main block is moved out to UB, and the tail block is temporarily stored in the unaligned register [0, unalign_count].
When this API is executed, the tail block is moved out from the unaligned register to UB.

## Prototype

- No offset specified; used with the API that does not specify a storage offset in [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md).

    ```cpp
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src)
    ```

- Uses int32_t as the offset with the [asc_storeunalign](./asc_storeunalign.md) API.

    ```cpp
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ hifloat8_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, int32_t offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, int32_t offset)
    ```

- Uses iter_reg as the offset with the API that uses iter_reg as the storage offset in [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md).

    ```cpp
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src, iter_reg offset)
    __simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src, iter_reg offset)
    ```

## Parameters

- Unspecified offset

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src | Input | Unaligned register used to store unaligned data, with a length of 32B. |

- int32_t as the storage offset

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src | Input | Unaligned register used to save unaligned data, with a length of 32B. |
    | offset | Input | Offset of the end address of the destination operand, of type int32_t, in units of element count. |

- iter_reg as the storage offset

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Starting address of the destination operand (vector). |
    | src | Input | Unaligned register used to save unaligned data, with a length of 32B. |
    | offset | Input | Offset of the end address of the destination operand, of type address register, in units of element count. |

For details about unaligned registers and address registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The **dst** in this API does not need to be 32B-aligned, but **dst** of data type **T** must be aligned to **sizeof(T)** bytes.
- Before calling this API, call [asc_storeunalign](./asc_storeunalign.md) or [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md), and the unaligned register of this API must be consistent with that of the preceding API.
- Ensure that the address of the destination operand plus the offset address corresponding to **offset** equals the end address of the data transfer. For details, see the example.

## Example

- Unspecified offset

    ```cpp
    // The dst address is 8, which is not 32B-aligned. The range is [8:520].
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```

- Use int32_t as the storage offset

    ```cpp
    // The dst address is 8, which is not 32B-aligned. The occupied address is UB[8:520].
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t count = 64;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        // Other reg operations

        // First time: src[0:62] is written to dst[0:62] (that is, UB[8:256]); meanwhile, src[62:64] is written to ureg[0:2]
        // Second time: ureg[0:2] + src[0:62] is written to dst[62:126] (that is, UB[256:512]); meanwhile, src[62:64] is written to ureg[0:2]
        asc_storeunalign(dst + i*count, ureg, src, count);
    }
    // The end address of dst is 520, that is, dst + repeat * count * sizeof(uint32_t), so offset must be configured as repeat * count.
    int32_t offset = repeat * count;
    // ureg[0:2] is written to dst[126:128] (that is, UB[512:520]).
    asc_storeunalign_post(dst, ureg, offset);
    ```

- Use iter_reg as the storage offset.

    ```cpp
    // The address of dst is 8, which is not 32B-aligned. The occupied address range is [8:520].
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
    vector_store_unalign ureg;
    vector_uint32_t src;
    uint32_t repeat = 2;
    for (uint32_t i = 0; i < repeat; i++) {
        iter_reg count = asc_create_iter_reg_b32(64);
        // Other reg operations.

        // First time: src[0:62] is written to dst[0:62] (that is, UB[8:256]); at the same time, src[62:64] is written to ureg[0:2]; at the same time, dst is incremented to dst[64] (that is, UB[264]).
        // Second time: ureg[0:2] + src[0:62] is written to dst[-2:62] (that is, UB[256:512]); at the same time, src[62:64] is written to ureg[0:2]; at the same time, dst is incremented to dst[128] (that is, UB[520]).
        asc_storeunalign_postupdate(dst, ureg, src, count);
    }
    // The end address of dst is 520, but dst has already been incremented to 520, so offset must be configured as 0.
    iter_reg offset = asc_create_iter_reg_b32(0);
    // ureg[0:2] is written to dst[-2:0] (that is, UB[512:520]).
    asc_storeunalign_post(dst, ureg, offset);
    ```
