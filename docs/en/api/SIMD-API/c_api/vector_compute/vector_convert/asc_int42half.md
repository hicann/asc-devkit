---
okftype: reference.api
description: "Converts int4b_t data to half, without rounding modes, with two overloads: first-n-data computation and high-dimensional slicing computation."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "General address overlap constraints and 32-byte start address alignment requirements."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The declaration location moved from include/c_api/vector_compute/vector_compute.h (9.1.0 and earlier) to include/c_api/vector_compute/compute/vector_convert.h (9.2.0 and later), consistent with the header file path in the description."
---
# asc_int42half

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.836254+00:00 pushedAt=2026-11-16T02:10:58.703Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/vector_compute/compute/vector_convert.h"`.

Converts int4_t data to the half type.

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_int42half(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_int42half(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- When src is int4b_t, the count of the first-n-data computation API and the synchronous computation API must be an even number.

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation.
uint64_t offset = 0;
__ubuf__ int4b_t* src = (__ubuf__ int4b_t*)asc_get_phy_buf_addr(offset);
offset += total_length / 2;    // One element of int4b_t occupies half a byte.
__ubuf__ half* dst = (__ubuf__ half*)asc_get_phy_buf_addr(offset);
asc_int42half(dst, src, total_length);    // Convert src to the half type and store the result in dst.
```
