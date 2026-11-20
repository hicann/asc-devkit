---
okftype: reference.api
description: "Deprecated synchronous type conversion interface that converts an int16_t source vector to float by count elements and writes to dst. It runs on PIPE_V and is supported only on the 2201 architecture. Use asc_int162float together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_int162float.md
    kind: replaced_by
    condition: "Replaces the int16 to float conversion capability of this interface; synchronization must be inserted separately"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "include/c_api/composite/vector_compute_composite.h:2783 is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int162float and asc_sync). The body text switches to the non-synchronous interface plus asc_sync. The declaration is still retained in 9.3.0-beta.1, which is deprecation rather than removal, with no upper version limit."
---
# asc_int162float_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:42:09.489Z pushedAt=2026-11-18T03:12:16.543Z nl=2 -->

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

**The `asc_int162float_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_convert/asc_int162float.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts int16_t data to float.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_int162float_sync(__ubuf__ float* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the computation.
__ubuf__ int16_t src[total_length];
__ubuf__ float dst[total_length];
asc_int162float_sync(dst, src, total_length);    // Convert src to float and store the result in dst.
```
