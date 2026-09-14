# asc_transpose

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:30:10.549Z pushedAt=2026-09-08T03:45:05.588Z -->

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

Used to implement the transpose of a 16*16 two-dimensional matrix data block.

## Prototype

  ```cpp
    __aicore__ inline void asc_transpose(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
    __aicore__ inline void asc_transpose(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
  ```

- Synchronous computation
  ```cpp
    __aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
    __aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
  ```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
// total_length indicates the total length of the data involved in the computation.
constexpr int total_length = 256; 
__ubuf__ int16_t dst[total_length];
__ubuf__ int16_t src[total_length];
// dst indicates the address of the destination operand.
asc_transpose(dst, src);
```