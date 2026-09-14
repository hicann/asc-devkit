# asc_mrgsort4

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:51:25.528Z pushedAt=2026-09-08T03:45:05.545Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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

Merges up to four sorted queues into a single queue, with the results sorted by the score field in descending order.

The data processed by **asc_mrgsort4** is generally the output of the **asc_bitsort** instruction. The structure of the queue, Region Proposal, is as follows:
- The data type is float, and each structure occupies 8 bytes.

![ ](../figures/mrgsort4_01.png)

- The data type is half, and each structure also occupies 8 bytes, with 2 bytes reserved in the middle.

![ ](../figures/mrgsort4_02.png)

## Prototype

- Computation

    ```cpp
    __aicore__ inline void asc_mrgsort4(__ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    __aicore__ inline void asc_mrgsort4(__ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mrgsort4_sync(__ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    __aicore__ inline void asc_mrgsort4_sync(__ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    ```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Array of start addresses of the source operands (vectors), that is, the Region Proposal queues to be merged. The number of queues, **ASC_C_API_MRGSORT_ELEMENT_LEN**, is 4. |
| repeat | Input | Number of iterations. In each iteration, the source operands and the destination operand are computed and the total length of four queues is skipped. |
| element_length_0 | Input | Length of source queue 0. |
| element_length_1 | Input | Length of source queue 1. |
| element_length_2 | Input | Length of source queue 2. |
| element_length_3 | Input | Length of source queue 3. |
| if_exhausted_suspension | Input | Whether the instruction needs to stop after a queue is exhausted. |
| valid_bit | Input | Number of valid queues. Valid values are as follows:<br>**3**: The first two queues are valid.<br>**7**: The first three queues are valid.<br>**15**: All four queues are valid. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When **repeat** is 1, a single iteration is performed, and the **element_length_...**, **if_exhausted_suspension**, and **valid_bit** parameters can be set freely.
- When **repeat** is greater than 1, multiple iterations are performed, and the following conditions must be met: four source queues are included, the four source queues have the same length and are stored contiguously in memory, the value of **valid_bit** is 15, and the value of **if_exhausted_suspension** is false.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
// The example performs merge sort on eight sorted Region Proposal queues, each containing 32 elements.
// Step 1: Sort four 32-element queues src at a time, with repeat performed twice, to obtain two sorted 128-element new queues tmp.
// Step 2: Perform merge sort on the two sorted 128-element new queues tmp again to obtain the final 256-element queue dst.

constexpr uint16_t element_length = 32;
// Each element contains a float32 score and a 4-byte index, occupying 8 bytes, equivalent to 2 sizeof(float).
constexpr uint16_t array_length = element_length * 2;
__ubuf__ float src_workspace[array_length * 8];
__ubuf__ float tmp_workspace[array_length * 8];
__ubuf__ float dst_workspace[array_length * 8];

__ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN];
__ubuf__ float* dst;
uint8_t valid_bit;

// Each iteration can process four queues, for a total of two iterations.
src[0] = &src_workspace[0];
src[1] = &src_workspace[1 * array_length];
src[2] = &src_workspace[2 * array_length];
src[3] = &src_workspace[3 * array_length];
dst = tmp_workspace;
valid_bit = 15;
asc_mrgsort4(dst, src, 2, element_length, element_length, element_length, element_length, false, valid_bit);

// Perform one iteration to process two queues.
src[0] = &tmp_workspace[0];
src[1] = &tmp_workspace[1 * 4 * array_length];
dst = dst_workspace;
valid_bit = 3;
asc_mrgsort4(dst, src, 1, 4 * element_length, 4 * element_length, 0, 0, false, valid_bit);
```
