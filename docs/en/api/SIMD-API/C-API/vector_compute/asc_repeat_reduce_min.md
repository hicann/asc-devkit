# asc_repeat_reduce_min

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:09:18.956Z pushedAt=2026-09-08T03:45:05.574Z -->

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

Computes the minimum value of all elements in each Repeat, as well as the index of the first minimum element within the current Repeat. Each Repeat computation processes 256 bytes.

**index_value** API: Indicates that the index is in the lower half, and the return result storage order is [index, value]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
**value_index** API: Indicates that the value is in the lower half, and the return result storage order is [value, index]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
**only_value** API: Indicates that only the extreme value is returned, and the return result storage order is [value].
**only_index** API: Indicates that only the index of the extreme value is returned, and the return result storage order is [index]. The index data type is fixed to uint32.

After executing this API, you can continue to execute [asc_get_reduce_min_cnt](./asc_get_reduce_min_cnt.md) to obtain the minimum value across all repeats and the index of the first minimum value.

## Prototype

- Computation of the first n data elements

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

- High-dimensional split computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride, uint16_t src_repeat_stride)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

Table 1 Parameter description

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations.|
| dst_repeat_stride | Input | Address stride between adjacent iterations of the destination operand, in units of the length computed by one repeat, as follows:<br>When both the extreme value and index are returned, if the output data type is T, the unit is sizeof(T)*2.<br>When only the extreme value is returned and the input type is T, the unit is sizeof(T).<br>When only the index is returned, the unit is 4 bytes (sizeof(uint32)). |
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand.|
| src_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the source operand.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The index data in the output is stored according to the data type of dst. For example, the index data of a float output should be uint32, so when reading it, the float must be reinterpreted as uint32.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start address of dst requires 4-byte alignment when the input type is half, and 8-byte alignment when the input type is float.
- The start address of **src** must be 32-byte aligned.


## Example

```cpp
// Compute the minimum values and indices of 256 half elements in two iterations, producing two value_index results.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 4;
__ubuf__ half dst[dst_length];

uint8_t repeat = 2;
// The unit is sizeof(half)*2 = 4 bytes, stored contiguously.
uint16_t dst_repeat_stride = 1;
uint16_t src_block_stride = 1;
uint16_t src_repeat_stride = 8;
asc_repeat_reduce_min_value_index(dst, src, repeat, dst_repeat_stride, src_block_stride, src_repeat_stride);
```

Result example:

```
Input data src: [256, 255, 254, ..., 129, 128, ..., 2, 1]
Output data dst: [129, 7.6e-6, 1, 7.6e-6]
Description: The binary representation of 7.6e-6 of the half type is 0x007F. When reinterpreted as uint32, it represents the index of the minimum value, which is 127.
```
