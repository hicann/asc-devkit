---
okftype: reference.api
description: "Deprecated API page: On the 3510 architecture, asc_copy_l12fb_sync synchronously moves the quantization parameters from L1 Buffer to Fixpipe Buffer, with the pipeline type PIPE_FIX. It is recommended to use the non-synchronous asc_copy_l12fb together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space, alignment, and address overlap constraints follow the storage unit description and the general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronous waiting in the alternative solution"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 3510 synchronous form with ASC_DEPRECATED(9.2.0). The replacement path is the non-synchronous asc_copy_l12fb plus asc_sync; the interface is not removed."
---
# asc_copy_l12fb_sync (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:06:50.686Z pushedAt=2026-11-18T03:12:16.424Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

The header file path is `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12fb_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md) together with the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from the L1 Buffer to the Fixpipe Buffer, which is used to store quantization parameters.

## Prototype

- Synchronous move

    ```cpp
    __aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size | Input | Size of the data to move (in bytes). |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
// The synchronous API moves data continuously by byte count. Here a 32-byte data block is moved.
constexpr uint32_t copy_size = 32;
__cbuf__ half src[256];
__fbuf__ half dst[256];
asc_copy_l12fb_sync(dst, src, copy_size);
```
