---
okftype: reference.api
description: "Deprecated sparse matrix multiply-accumulate legacy prototype, where unit_flag is uint8_t, the left sparse matrix A is densified during computation, the right matrix B must be pre-densified, and it includes the corresponding asc_mmad_sparse_sync synchronization form. The replacement is the strongly-typed unit_flag_mode prototype."
tag:
  - roles: [compute]
  - operations: [matrix_multiply_accumulate, synchronization]
  - traits: [sparse_input]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_sparse.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_unit_flag_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The old asc_mmad_sparse prototype with unit_flag as uint8_t and asc_mmad_sparse_sync are deprecated. Use the current interface of the same name with unit_flag_mode as asc_unit_flag_mode together with asc_sync; both declarations in include/c_api/cube_compute/cube_compute.h and include/c_api/composite/cube_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_mmad_sparse (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:19:33.027Z pushedAt=2026-11-18T03:12:16.611Z nl=2 -->

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

The header file paths are: `"c_api/cube_compute/cube_compute.h"`, `"c_api/composite/cube_compute_composite.h"`.

**The `asc_mmad_sparse` interface whose input parameter `unit_flag` is of type `uint8_t` and the `asc_mmad_sparse_sync` interface are deprecated. Use the [asc_mmad_sparse](../cube_compute/mmad_compute/asc_mmad_sparse.md) interface whose input parameter `unit_flag_mode` is of type `asc_unit_flag_mode` instead. For the synchronization functionality of the original `asc_mmad_sparse_sync` interface, implement it through the new `asc_mmad_sparse` interface and `asc_sync()`. For details, see [asc_sync](../sync/intra_core_sync/asc_sync.md).**

This interface performs a sparse matrix multiply-accumulate operation. The input left matrix A is a sparse matrix, and the right matrix B is a dense matrix. Matrix A is densified during computation, and matrix B must be densified during the input data preparation stage.

This interface is a cube computation interface and takes effect only on the AIC.

## Prototype

```c
__aicore__ inline void asc_mmad_sparse(__cc__ int32_t* c,
                                       __ca__ int8_t* a,
                                       __cb__ int8_t* b,
                                       uint16_t m,
                                       uint16_t k,
                                       uint16_t n,
                                       uint8_t unit_flag,
                                       bool c_matrix_source,
                                       bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sparse_sync(__cc__ int32_t* c,
                                            __ca__ int8_t* a,
                                            __cb__ int8_t* b,
                                            uint16_t m,
                                            uint16_t k,
                                            uint16_t n,
                                            uint8_t unit_flag,
                                            bool c_matrix_source,
                                            bool c_matrix_init_val)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| c | Output | Starting address of the result matrix C in the L0C Buffer, which must be 1024-byte aligned. |
| a | Input | Starting address of the left matrix A in the L0A Buffer, which must be 512-byte aligned. |
| b | Input | Starting address of the right matrix B in the L0B Buffer, which must be 512-byte aligned. |
| m | Input | M dimension size of the left matrix A and the result matrix C, with a value range of [0, 4095]. |
| k | Input | K dimension size of the left matrix A and the right matrix B, with a value range of [0, 4095]. |
| n | Input | N dimension size of the right matrix B and the result matrix C, with a value range of [0, 4095]. |
| unit_flag | Input | UnitFlag control parameter. 0 indicates disabled; 2 indicates enabled and the unit flag is retained after execution; 3 indicates enabled and the unit flag is updated after execution. |
| c_matrix_source | Input | When `c_matrix_init_val` is false, configures whether the initial value of matrix C comes from the BiasTable Buffer. |
| c_matrix_init_val | Input | Configures whether to set the initial value of matrix C to 0. |

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- This interface takes effect only on the AIC. Calling it on the AIV returns directly.
- Every 4 elements in the original sparse matrix B contain at most 2 non-zero elements. If there are 3 or more non-zero elements, only the first 2 non-zero elements are used.
- If any one of `m`, `k`, and `n` is 0, the interface is treated as a NOP (no operation).
- For sparse matrix data preparation, dense algorithms, and other constraints, see [asc_mmad_sparse](../cube_compute/mmad_compute/asc_mmad_sparse.md).
