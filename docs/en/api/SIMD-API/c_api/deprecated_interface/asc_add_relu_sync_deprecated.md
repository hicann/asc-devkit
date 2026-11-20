---
okftype: reference.api
description: "Deprecated vector add plus ReLU synchronous interface. It first sums two source vectors element-wise, then compares the result with 0 and takes the larger value, and supports destination type conversion combinations. It is supported only on Atlas A2 and A3. The replacement is asc_add_relu used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [add, relu, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_add_relu.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The composite synchronous interface family is deprecated as a whole; use asc_add_relu and asc_sync in separate calls instead. All 6 declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_add_relu_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:14:04.996Z pushedAt=2026-11-18T08:20:52.056Z nl=2 -->

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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_add_relu_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_add_relu.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs element-wise summation, then applies ReLU computation (compares the result with 0 and takes the larger value), and provides the capability to convert the data type of the final result (s162s8, f322f16, f162s8). The calculation formula is as follows:

$$
dst_i = relu(src0_i + src1_i)
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    __aicore__ inline void asc_add_relu_sync(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description                                |
|:----------| :-----|:----------------------------------|
| dst       | Output | Start address of the destination operand (vector).                   |
| src0, src1 | Input | Start address of the source operand (vector).                        |
| count     | Input | Number of elements involved in the computation.                        |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// The data types of the destination operand and source operands are the same.
// total_length refers to the total length of the data involved in the computation.
constexpr uint64_t total_length = 128;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];
asc_add_relu_sync(dst, src0, src1, total_length);
```

```cpp
// The data types of the destination operand and source operands are different (s162s8).
// total_length refers to the length of the data involved in the computation.
constexpr uint32_t total_length = 32;
__ubuf__ int8_t dst[total_length];
__ubuf__ int16_t src0[total_length];
__ubuf__ int16_t src1[total_length];
asc_add_relu_sync(dst, src0, src1, total_length);
```
