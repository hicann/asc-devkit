---
okftype: reference.api
description: "Deprecated synchronous multiply-cast interface that multiplies two half source vectors element by element, then rounds with RINT and saturates to uint8_t. Supported only on Atlas A2 and A3. The replacement is asc_mul_cast_half2uint8 combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion, multiply, synchronization]
  - traits: [rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_mul_cast_half2uint8.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The sync composite interface is deprecated. Use asc_mul_cast_half2uint8 and asc_sync instead. The declaration in include/c_api/composite/vector_compute_composite.h carries ASC_DEPRECATED(9.2.0)."
---
# asc_mul_cast_half2uint8_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:25:43.288Z pushedAt=2026-11-18T03:12:16.618Z nl=2 -->

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

**The `asc_mul_cast_half2uint8_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_mul_cast_half2uint8.md) together with the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Computes the product element by element and converts the result to the uint8_t type. The calculation formula is as follows:
$$
dst_i = Cast(src0_i * src1_i)
$$

- Precision conversion rule: The product of the source operands is rounded using the RINT rounding mode (round half to even), and overflow is handled by saturation.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mul_cast_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of source operand 0 (vector). |
| src1 | Input | Start address of source operand 1 (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// total_length refers to the data length involved in the computation
constexpr uint32_t total_length = 128;
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ uint8_t dst[total_length];
asc_mul_cast_half2uint8_sync(dst, src0, src1, total_length);
```
