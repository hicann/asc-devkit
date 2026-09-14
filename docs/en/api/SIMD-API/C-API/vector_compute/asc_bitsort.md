# asc_bitsort

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:39:09.264Z pushedAt=2026-09-08T03:45:05.453Z -->

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

A bitmap sort function that sorts 32 elements in one iteration. The data is stored in the structure described as follows:

Score and Index are stored in **src0** and **src1**, respectively. The elements are sorted by Score (elements with larger Score values are placed first), and the sorted Score together with its corresponding Index are stored in **dst** as a (Score, Index) structure.

## Prototype

- Continuous computation
  ```cpp
  __aicore__ inline void asc_bitsort(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  __aicore__ inline void asc_bitsort(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  ```

- Synchronous computation
  ```cpp
  __aicore__ inline void asc_bitsort_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  __aicore__ inline void asc_bitsort_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Start address of the destination operand (vector). |
| src0      | Input    | Start address of the source operand (vector). |
| src1      | Input    | Start address of the source operand (vector). |
| repeat    | Input    | Number of repeated iterations. Each iteration sorts 32 elements. In the next iteration, **src0** and **src1** each skip 32 elements, and **dst** skips 256 bytes of space. Value range: [0, 255]. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
__ubuf__ half dst[256];
__ubuf__ half src0[256];
__ubuf__ uint32_t src1[256];
asc_bitsort(dst, src0, src1, 8);
```
