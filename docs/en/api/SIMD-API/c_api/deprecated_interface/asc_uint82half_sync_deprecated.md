---
okftype: reference.api
description: "Deprecated API page: on the 2201 architecture, asc_uint82half_sync synchronously widens a uint8_t vector to half and writes it to dst. dst and src must be 32-byte aligned. It is recommended to use the non-synchronous asc_uint82half plus asc_sync instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Performs synchronous waiting in the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_uint82half.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks ASC_DEPRECATED(9.2.0) before the asc_uint82half_sync declaration. Release Notes 9.2.0 lists the _sync composite interface as deprecated as a whole; the interface has not been removed."
---
# asc_uint82half_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:19:40.711Z pushedAt=2026-11-18T03:12:16.700Z nl=2 -->

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

**The `asc_uint82half_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_convert/asc_uint82half.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts uint8_t data to half type.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_uint82half_sync(__ubuf__ half* dst, __ubuf__ uint8_t* src, uint32_t count)
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
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation
__ubuf__ uint8_t src[total_length];
__ubuf__ half dst[total_length];
asc_uint82half_sync(dst, src, total_length);    // Convert src to half type and store the result in dst
```
