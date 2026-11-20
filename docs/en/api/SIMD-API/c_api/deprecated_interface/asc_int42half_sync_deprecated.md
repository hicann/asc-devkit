---
okftype: reference.api
description: "Deprecated synchronous type conversion interface that converts packed int4b_t data to half and computes along the PIPE_V pipeline. The replacement is the corresponding non-synchronous interface asc_int42half used together with asc_sync for synchronization."
tag:
  - roles: [compute]
  - operations: [conversion]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the generic address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Used together with the non-synchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_int42half.md
    kind: replaced_by
    condition: "The non-synchronous interface that replaces this synchronous interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The declaration is marked as deprecated with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int42half and asc_sync) (the macro line before @override_id asc_int42half_sync__PUf16_PUi4x2_u32 in include/c_api/composite/vector_compute_composite.h). The replacement is the corresponding non-synchronous interface asc_int42half plus asc_sync. This page contains only this one prototype, so the entire page is deprecated, and no availability upper limit is set."
---
# asc_int42half_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:52:48.086Z pushedAt=2026-11-18T03:12:16.565Z nl=2 -->

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

**The `asc_int42half_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_convert/asc_int42half.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts int4_t data to the half type.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_int42half_sync(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count)
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
- When src is int4b_t, the count of the first n data computation interfaces and the synchronous computation interface must be an even number.

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the computation.
uint64_t offset = 0;
__ubuf__ int4b_t* src = (__ubuf__ int4b_t*)asc_get_phy_buf_addr(offset);
offset += total_length / 2;    // One element of int4b_t occupies half a byte.
__ubuf__ half* dst = (__ubuf__ half*)asc_get_phy_buf_addr(offset);
asc_int42half_sync(dst, src, total_length);    // Convert src to the half type and store the result in dst.
```
