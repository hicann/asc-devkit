---
okftype: reference.api
description: "Deprecated vector bitwise AND synchronous interface that performs bitwise AND element-wise on two source vectors of int16_t and uint16_t and writes the result to the destination vector. It is supported only on Atlas A2 and A3, and the replacement is asc_and combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [bitwise_and, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_logic/asc_and.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The composite synchronous interface family is deprecated; use asc_and and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h:3018,3033 carry ASC_DEPRECATED(9.2.0)."
---
# asc_and_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:14:04.205Z pushedAt=2026-11-18T03:12:16.359Z nl=2 -->

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

**The `asc_and_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_logic/asc_and.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs vector bitwise AND. The calculation formula is as follows:

$$
dst_i = src0_i \quad \&  \quad src1_i
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_and_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
    __aicore__ inline void asc_and_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Destination operand.            |
| src0, src1 | Input    | Start address of the source operand (vector).             |
| count     | Input    | Number of elements involved in the computation.        |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.

## Examples

```cpp
constexpr uint64_t total_length = 64;
//total_length indicates the data length involved in the computation.
__ubuf__ int16_t src0[total_length];
__ubuf__ int16_t src1[total_length];
__ubuf__ int16_t dst[total_length];
asc_and_sync(dst, src0, src1, total_length);
```
