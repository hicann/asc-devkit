---
okftype: reference.api
description: "Deprecated API page: Under the 2201 architecture, asc_copy_l12fb_sync synchronously copies quantization parameters from L1 Buffer to Fixpipe Buffer using the destination address, source address, and byte count. The first n data movement requires 32-byte alignment. It is recommended to use the non-synchronous asc_copy_l12fb together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12fb_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space, alignment, and address overlap constraints follow its storage unit description and general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "The alternative solution performs the synchronous wait"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 2201 synchronous form with ASC_DEPRECATED(9.2.0). The alternative path is the non-synchronous asc_copy_l12fb plus asc_sync; the interface has not been removed."
---
# asc_copy_l12fb_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:04:55.834Z pushedAt=2026-11-18T03:12:16.422Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12fb_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_2201.md) together with the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from the L1 Buffer to the Fixpipe Buffer. The Fixpipe Buffer is used to store quantization parameters.

## Prototype

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size | Input | Size of the data to be moved (in bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- When the first n data movement interfaces are used, the size of the data to be moved must be 32-byte aligned.

## Examples

```cpp
// The synchronous interface moves data continuously by byte count. Here a 32-byte data block is moved.
constexpr uint32_t copy_size = 32;
__cbuf__ half src[256];
__fbuf__ half dst[256];
asc_copy_l12fb_sync(dst, src, copy_size);
```
