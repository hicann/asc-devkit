# asc_reduce

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:06:34.775Z pushedAt=2026-09-08T03:45:05.567Z -->

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

Selects elements from the source operand and writes them into the destination operand based on the gather mask (data collection mask) corresponding to the built-in fixed pattern in binary form or the user-defined input value.

## Prototype

- Computation of the first n data elements

    ```c++
    __aicore__ inline void asc_reduce(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
    __aicore__ inline void asc_reduce(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count)
    ```

- High-dimensional split computation

  ```c++
  __aicore__ inline void asc_reduce(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1,
    uint16_t repeat, uint8_t src0_block_stride, uint8_t pattern_mode, uint16_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_reduce(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1,
    uint16_t repeat, uint8_t src0_block_stride, uint8_t pattern_mode, uint16_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

- Synchronous computation

  ```c++
  __aicore__ inline void asc_reduce_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
  __aicore__ inline void asc_reduce_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count)
  ```

## Parameters

Table 1 Parameter description

| Parameter                | Input/Output | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|:-------------------| :-----|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst                | Output | Start address of the destination operand (vector).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| src0               | Input | Start address of the source operand (vector).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| src1               | Input | In user-defined mode, the user-defined data collection mask.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| count              | Input | Number of elements involved in the computation.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| repeat             | Input | Number of iterations.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| src0_block_stride  | Input | Address stride between different data blocks within a single iteration of source operand 0.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| pattern_mode       | Input | Gather mask (data collection mask), which is divided into a built-in fixed pattern and a user-defined pattern. Elements are selected from the source operand and written into the destination operand based on the binary form corresponding to the built-in fixed pattern or the binary form corresponding to the user-defined input value.<br/>&bull;Built-in fixed pattern: The data type of **pattern_mode** is **uint8_t**, with a value range of [1, 7]. All iterations use the same gather mask. Configuring **src1** is not supported.<br/>&nbsp;&nbsp;&bull; 1: 01010101...0101 # Selects even-indexed elements in each repeat.<br/>&nbsp;&nbsp;&bull; 2: 10101010...1010 # Selects odd-indexed elements in each repeat.<br/>&nbsp;&nbsp;&bull; 3: 00010001...0001 # Selects the first element of every four elements in each repeat.<br/>&nbsp;&nbsp;&bull; 4: 00100010...0010 # Selects the second element of every four elements in each repeat.<br/>&nbsp;&nbsp;&bull; 5: 01000100...0100 # Selects the third element of every four elements in each repeat.<br/>&nbsp;&nbsp;&bull; 6: 10001000...1000 # Selects the fourth element of every four elements in each repeat.<br/>&nbsp;&nbsp;&bull; 7: 11111111...1111 # Selects all elements in each repeat.<br/>&bull;User-defined pattern: When **pattern_mode** is set to 0, the user-defined pattern is used. The user inputs a custom gather mask through the **src1** parameter, and the iteration interval is determined by **src1_repeat_stride**. |
| src0_repeat_stride | Input | Address stride of the same data block between adjacent iterations of source operand 0.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| src1_repeat_stride | Input | Address stride of the same data block between adjacent iterations of source operand 1.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The API does not support configuring the mask through [asc_set_vector_mask](asc_set_vector_mask.md).
- The start addresses of **dst**, **src0**, and **src1** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
//total_length refers to the total length of data involved in the computation.
constexpr uint64_t total_length = 128;
__ubuf__ uint16_t src0[total_length];
__ubuf__ uint16_t src1[total_length/16];
__ubuf__ uint16_t dst[total_length];
asc_reduce(dst, src0, src1, total_length);
```
