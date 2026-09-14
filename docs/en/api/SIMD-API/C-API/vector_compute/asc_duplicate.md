# asc_duplicate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:48:33.660Z pushedAt=2026-09-08T03:45:05.464Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Performs a vector duplicate operation to copy and fill a scalar value into a vector.

## Prototype

- Computation of the first n data elements

    ```cpp
    __aicore__ inline void asc_duplicate(__ubuf__ half* dst, half src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ int16_t* dst, int16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ float* dst, float src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ int32_t* dst, int32_t src, uint32_t count)
    __aicore__ inline void asc_duplicate(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_duplicate(__ubuf__ half* dst, half src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ int16_t* dst, int16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ uint16_t* dst, uint16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ float* dst, float src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ int32_t* dst, int32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    __aicore__ inline void asc_duplicate(__ubuf__ uint32_t* dst, uint32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_duplicate_sync(__ubuf__ half* dst, half src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ int16_t* dst, int16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ float* dst, float src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ int32_t* dst, int32_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| **dst** | Output | Start address of the destination operand (vector). |
| **src** | Input | Source operand (scalar). |
| **count** | Input | Number of elements involved in continuous duplication. |
| **repeat** | Input | Number of iterations. |
| **dst_block_stride** | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| **dst_repeat_stride** | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of **dst** must be 32-byte aligned.

## Example

```cpp
constexpr uint32_t dst_length = 128;
__ubuf__ uint16_t dst[dst_length];
uint16_t val = 18;
asc_duplicate(dst, val, dst_length);
```

Result example:

```
Output data dst: [18 18 18 ... 18]
```