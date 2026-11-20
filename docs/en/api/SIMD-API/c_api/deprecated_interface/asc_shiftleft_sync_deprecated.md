---
okftype: reference.api
description: "Deprecated API page: Under the 2201 architecture, asc_shiftleft_sync shifts all vector elements left by distance bits and writes the result to dst. distance cannot be negative and must not exceed the bit width of the source type. dst and src must be 32-byte aligned. It is recommended to use the asynchronous asc_shiftleft together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [bitwise_shift]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronous waiting in the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_logic/asc_shiftleft.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0) before the asc_shiftleft_sync declaration. Release Notes 9.2.0 lists the _sync composite interfaces for matrix computation, matrix transfer, vector data transfer, and Memory vector computation as deprecated as a whole; the interface is not removed."
---
# asc_shiftleft_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:57:01.199Z pushedAt=2026-11-18T03:12:16.670Z nl=2 -->

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

**The `asc_shiftleft_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_logic/asc_shiftleft.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Shifts all elements left by distance bits. The calculation formula is as follows:
$$
dst_i = src_i << distance
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count)
    __aicore__ inline void asc_shiftleft_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| distance | Input | Source operand (scalar), indicating the number of bits to shift left. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- distance does not support being set to a negative number.
- The value of distance cannot exceed the bit width of the src data type.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;
uint32_t distance = 1;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_shiftleft_sync(dst, src, distance, total_length);
```
