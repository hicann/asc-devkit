---
okftype: reference.api
description: "Quantizes the int32_t source vector element-wise according to the scale set by asc_set_deq_scale and converts it to half, providing two forms: computation on the first n elements and high-dimensional slicing computation. Must be used with the non-vector mode of this API."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Must comply with the general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: requires
    condition: "Must be used with the non-vector mode of this API to set quantization parameters"
version: ">=9.0.0"
version_changes: []
---

# asc_deq_int322half

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.863318+00:00 pushedAt=2026-11-16T02:10:58.787Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_fused.h"`.

Quantizes the input int32_t data element-wise and converts it to the half type. The calculation formula is as follows:
$$
dst_i = Cast(src_i * scale)
$$
Here, scale can be set through [asc_set_deq_scale](asc_set_deq_scale.md).

## Prototype

- Computation on the first n elements

    ```cpp
    __aicore__ inline void asc_deq_int322half(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count)
    ```

- High-dimensional slicing computation

    ```cpp
    __aicore__ inline void asc_deq_int322half(__ubuf__ half* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint8_t dst_repeat_stride, uint8_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
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

- Must be used with the non-vector mode of the [asc_set_deq_scale](asc_set_deq_scale.md) API, which is used to set the quantization parameters.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation.
__ubuf__ int32_t src[total_length];
__ubuf__ half dst[total_length];
asc_set_deq_scale(2.0);
…… // data movement and synchronization operations.
asc_deq_int322half(dst, src, total_length);
```
