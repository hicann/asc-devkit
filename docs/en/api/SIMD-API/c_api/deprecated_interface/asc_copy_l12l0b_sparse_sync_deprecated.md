---
okftype: reference.api
description: "Deprecated interface page: On the 2201 architecture, asc_copy_l12l0b_sparse_sync moves the 512-byte dense weight matrix from the L1 Buffer to the L0B Buffer, and simultaneously reads the 128-byte index matrix to complete sparsification. The indexes are packed in reverse order. It is recommended to use the non-synchronous asc_copy_l12l0b_sparse together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer, sparse_input]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space, alignment, and address overlap constraints follow the storage unit description and the general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Performs synchronous waiting in the alternative solution"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0) before the asc_copy_l12l0b_sparse_sync declaration. The alternative path is the non-synchronous asc_copy_l12l0b_sparse plus asc_sync; the API has not been removed."
---
# asc_copy_l12l0b_sparse_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:19:42.856Z pushedAt=2026-11-18T03:12:16.433Z nl=2 -->

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

**The `asc_copy_l12l0b_sparse_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md) and the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

It is used to move a 512B dense weight matrix stored in the L1 Buffer to the L0B Buffer, and simultaneously read a 128B index matrix for sparsification of the dense matrix.

The index matrix is arranged in reverse order at an int8_t address. For example, the index matrix 1 2 0 1 0 2 1 0 is arranged at the address as 1 0 2 1 0 1 2 0, where 1 0 2 1 (corresponding to the first four digits 1 2 0 1 of the index matrix) is combined into one int8_t data, and 0 1 2 0 (corresponding to the last four digits 0 2 1 0 of the index matrix) is combined into one int8_t data.

## Prototype

- Synchronous move

  ```cpp
  __aicore__ inline void asc_copy_l12l0b_sparse_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand in the L0B Buffer. |
| src | Input | Start address of the source operand in the L1 Buffer. |
| index | Input | Start address of the index matrix in the L1 Buffer. |
| start_index | Input | Fractal matrix ID, indicating that the move start position is the Nth fractal in the source operand (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. Unit: 512B. |
| repeat | Input | Iteration count. Each iteration can process 512B data. Value range: [1, 255]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- If repeat is 0, the operation is not executed.
- start_index cannot be less than 0.
- Transposition is not supported.

## Examples

```cpp
__cb__ int8_t dst[256];
__cbuf__ int8_t src[256];
__cbuf__ int8_t index[64];
uint16_t start_index = 1;
uint8_t repeat = 8;
asc_copy_l12l0b_sparse_sync(dst, src, index, start_index, repeat);
```
