---
okftype: reference.api
description: "Moves the 512-byte fractal dense weight cube from L1 Buffer into L0B Buffer, and simultaneously reads the 128-byte index cube to complete sparsification for use by sparse cube multiply-accumulate."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [datablock_granularity, sparse_input]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_sparse.md
    kind: related
    condition: "Compressed weight and index cube for sparse cube multiply-accumulate reading"
version: ">=9.0.0"
version_changes: []
---
# asc_copy_l12l0b_sparse

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:57:32.954Z pushedAt=2026-11-18T08:15:34.284Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Used to move a 512B dense weight cube stored in the L1 Buffer to the L0B Buffer, while reading a 128B index cube for sparsification of the dense cube.

The index cube is arranged in reverse order within an int8_t address. For example, the index cube 1 2 0 1 0 2 1 0 is arranged in the address as 1 0 2 1 0 1 2 0, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the index cube) is combined into one int8_t value, and 0 1 2 0 (corresponding to the last four bits 0 2 1 0 of the index cube) is combined into one int8_t value.

For the data layout before moving, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md); for address alignment and move granularity requirements, see [Cube Computation Input Move Constraints](cube_compute_input_move_constraint.md), and use them together with the parameter and constraint descriptions of this API.

For how the compressed weight and the index cube work together, see [Auxiliary Cube Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/aux_cube_fractal_format_details.md); for the subsequent sparse cube multiply-add, see [asc_mmad_sparse](../../cube_compute/mmad_compute/asc_mmad_sparse.md).

## Prototype

- High-dimensional split move

  ```cpp
  __aicore__ inline void asc_copy_l12l0b_sparse(__cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand in the L0B Buffer. |
| src | Input | Start address of the source operand in the L1 Buffer. |
| index | Input | Start address of the index cube in the L1 Buffer. |
| start_index | Input | Fractal cube ID, indicating that the move start position is the Nth fractal in the source operand (0 is the first fractal cube in the source operand). Value range: [0, 65535]. Unit: 512B. |
| repeat | Input | Number of iterations, where each iteration can process 512B of data. Value range: [1, 255]. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- If repeat is 0, no operation is performed.
- start_index cannot be less than 0.
- Transposition is not supported.

## Examples

```cpp
__cb__ int8_t dst[256];
__cbuf__ int8_t src[256];
__cbuf__ int8_t index[64];
uint16_t start_index = 1;
uint8_t repeat = 8;
asc_copy_l12l0b_sparse(dst, src, index, start_index, repeat);
```
