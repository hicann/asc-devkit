# asc_gather

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:40:33.933Z pushedAt=2026-09-03T12:34:32.932Z -->

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

Gathers elements from the source operand **src** into the destination operand **dst** element-wise according to the index positions in **index**.

![asc_gather](../../figures/asc_gather.png)

## Prototype

- Gathers elements from the UB.

    ```cpp
    __simd_callee__ inline void asc_gather(vector_int16_t& dst, __ubuf__ int8_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_uint16_t& dst, __ubuf__ uint8_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_half& dst, __ubuf__ half* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_float& dst, __ubuf__ float* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint16_t index,vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint16_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_half& dst, __ubuf__ half* src, vector_uint32_t index, vector_bool mask)
    __simd_callee__ inline void asc_gather(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask)
    ```

- Gathers elements from the vector data register.

    ```cpp
    __simd_callee__ inline void asc_gather(vector_int8_t& dst, vector_int8_t src, vector_uint8_t index)
    __simd_callee__ inline void asc_gather(vector_uint8_t& dst, vector_uint8_t src, vector_uint8_t index)
    __simd_callee__ inline void asc_gather(vector_int16_t& dst, vector_int16_t src, vector_uint16_t index)
    __simd_callee__ inline void asc_gather(vector_uint16_t& dst, vector_uint16_t src, vector_uint16_t index)
    __simd_callee__ inline void asc_gather(vector_int32_t& dst, vector_int32_t src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather(vector_uint32_t& dst, vector_uint32_t src, vector_uint32_t index)
    __simd_callee__ inline void asc_gather(vector_half& dst, vector_half src, vector_uint16_t index)
    __simd_callee__ inline void asc_gather(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_uint16_t index)
    __simd_callee__ inline void asc_gather(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_uint8_t index)
    __simd_callee__ inline void asc_gather(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_uint8_t index)
    __simd_callee__ inline void asc_gather(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_uint8_t index)
    __simd_callee__ inline void asc_gather(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_uint8_t index)
    ```

## Parameters

- Gathers elements from the UB.

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Destination operand (vector data register). |
    | src | Input | Start address of the source operand (vector). |
    | index | Input | Data index (vector data register), indicating the index position of each element in **dst** relative to **src** in the UB. |
    | mask | Input | Source operand mask (mask register), indicating which elements participate in the computation. A value of 1 at the corresponding position indicates participation, and 0 indicates non-participation. Elements not selected by **mask** are set to zero in the output. |

- Gathers elements from the vector data register.

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Destination operand (vector data register). |
    | src | Input | Source operand (vector data register). |
    | index | Input | Data index (vector data register). |

For details about the vector data register and the mask register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- Gathering elements from UB

    When the data type of src is b8, dst is of the b16 data type. In this case, the lower 8 bits of the destination operand are the same as those of the source operand, and the upper 8 bits are automatically padded with 0. For example, when src is of the int8_t data type:

    40 = 0b00101000 -> 0b0000000000101000, which equals 40 after being extended to 16 bits

    -40 = 0b11011000 -> 0b0000000011011000, which equals 216 after being extended to 16 bits

- Gathering elements from vector data registers

    src is a vector data register with a fixed bit width of VL and a fixed number of elements. If an index value in index exceeds the maximum number of elements that the current vector data register can hold, it is handled as follows: Assume that the maximum number of data elements that the current vector data register can hold is vl_length and the index value in index is i. The index value is then updated to i % vl_length.

## Example

- Gathering elements from UB

    ```cpp
    vector_half dst;
    __ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
    vector_uint16_t index;
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    asc_loadalign(index, index_addr); // index_addr is the externally input UB memory space address.
    asc_gather(dst, src, index, mask);
    ```

- Gathering elements from vector data registers

    ```cpp
    vector_half dst;
    vector_half src;
    vector_uint16_t index;
    asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
    asc_loadalign(index, index_addr); // index_addr is the externally input UB memory space address.
    asc_gather(dst, src, index);
    ```
