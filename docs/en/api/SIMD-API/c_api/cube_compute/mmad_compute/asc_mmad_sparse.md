---
okftype: reference.api
description: "Sparse cube multiply-accumulate interface. The left cube A is a sparse cube and is densified during computation. The right cube B must be densified in advance and loaded together with the index cube through asc_copy_l12l0b_sparse. The index cube is then used for the densification of A. Only Atlas A3 and A2 products (2201 architecture) are supported. During computation, at most 2 non-zero elements are retained among every 4 elements."
tag:
  - roles: [compute]
  - operations: [matrix_multiply_accumulate]
  - traits: [sparse_input]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/cube_compute_fractal_intro/aux_cube_fractal_format_details.md
    kind: related
    condition: "Layout format of sparse weights and index cubes"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/overview/cube_compute_flow.md
    kind: related
    condition: "Flow from sparse data preparation to cube multiply-accumulate in sparse scenarios"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md
    kind: requires
    condition: "The dense cube B must be loaded into the L0B Buffer through this interface, and the generated index cube must be loaded at the same time"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:unit_flag_mode"
    scope_basis: resolved
    note: "The UnitFlag parameter is changed from uint8_t unitFlag to the strongly typed enum asc_unit_flag_mode, and the parameter names are changed from unitFlag/cmatrixSource/cmatrixInitVal to unit_flag_mode/c_matrix_source/c_matrix_init_val; the old uint8_t prototype is retained as a deprecated overload and is no longer displayed on this page (the upstream/9.2.0 declaration is consistent with the deprecated table in CANN_9_2_0.md)"
---
# asc_mmad_sparse

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:04:22.269Z pushedAt=2026-11-18T08:15:34.216Z nl=2 -->

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

The header file path is: `"c_api/cube_compute/cube_compute.h"`.

Performs a cube multiply-add operation, where the input left cube A is a sparse cube and the right cube B is a dense cube. For cube A, densification is completed during the asc_mmad_sparse computation; for cube B, densification is completed during input data preparation before computation execution (densification is performed according to the dense algorithm described below). The dense cube B needs to be loaded via [asc_copy_l12l0b_sparse](../../cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md), and the index cube is loaded at the same time. The index cube is generated during the densification of cube B and is then used for the densification of cube A.

For the layout of sparse weights and the index cube, see [Auxiliary Cube Fractal Format Details](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md); for the flow from sparse data preparation to cube multiply-add, see the sparse scenario in [Cube Computation Flow](../overview.md#2-out-of-4-sparse-cube-computation-flow).

## Prototype

```cpp
__aicore__ inline void asc_mmad_sparse(__cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, asc_unit_flag_mode unit_flag_mode, bool c_matrix_source, bool c_matrix_init_val)
```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| c       | Output    | 	Destination operand, result cube C, whose start address requires 1024-byte alignment.            |
| a       | Input    |    Source operand, left cube A, whose start address requires 512-byte alignment.|
| b       | Input    | 	Source operand, right cube B, whose start address requires 512-byte alignment.      |
| m       | Input    | 	Height of the left cube, value range: m∈[0,4095].      |
| k       | Input    | 	Width of the left cube and height of the right cube, value range: k∈[0,4095].      |
| n       | Input    | 	Width of the right cube, value range: n∈[0,4095].      |
| unit_flag_mode | Input | Used to control fine-grained parallelism between the cube multiply-add instruction and the cube move-out instruction. After UnitFlag is enabled, the hardware moves out the computation result each time it finishes computing one fractal. Value description is as follows:<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::DISABLE`: UnitFlag is not enabled.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_KEEP`: UnitFlag is enabled, and the hardware does not change the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; `asc_unit_flag_mode::ENABLE_UPDATE`: UnitFlag is enabled, and the hardware changes the unit flag bit after executing the instruction. |
| c_matrix_source       | Input    | 	Configures whether the initial value of cube C comes from the BiasTable Buffer (the hardware cache area that stores Bias). Value description is as follows:  <br>&bull; true: comes from the BiasTable Buffer. <br>&bull; false: comes from the L0C Buffer.    |
| c_matrix_init_val       | Input    | 	Configures whether the initial value of cube C is 0. Value description is as follows:  <br>&bull; true: the initial value of cube C is 0. <br>&bull; false: the initial value of cube C is configured through the c_matrix_source parameter.     |

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- In the original sparse cube B, at most 2 non-zero elements should be guaranteed among every 4 elements. If there are 3 or more non-zero elements, only the first 2 non-zero elements are used.
- When any one of m, k, and n is 0, this instruction is not executed.

## Dense Algorithm Description

Assume that among every 4 elements of the original sparse cube B, at least 2 are zero. The densified cube B is a dense cube in which 2 zeros are filtered out from every 4 elements. During the densification of cube B, an index cube is generated as follows: for every 4 elements in the sparse cube B, two 2-bit indexes are generated in the index cube and encoded according to the following rules. The indexes must be within the range {0, 1, 2}.
<br>&bull;The first index indicates the relative position of the first non-zero element among the first 3 elements.
<br>&bull;The second index indicates the relative position of the second non-zero element among the last 3 elements.
<br>For details, refer to the following table. Here, "-" indicates that the algorithm does not care about the value at that position, because it will be filtered out.
**Table 2** Dense algorithm description

<table border="1" style="border-collapse: collapse; width: 100%; text-align: center;">
  <!-- Row 1: not merged -->
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">Example</td>
    <td style="padding: 8px; border: 1px solid #333;">ele0</td>
    <td style="padding: 8px; border: 1px solid #333;">ele1</td>
    <td style="padding: 8px; border: 1px solid #333;">ele2</td>
    <td style="padding: 8px; border: 1px solid #333;">ele3</td>
    <td style="padding: 8px; border: 1px solid #333;">Index_a[i]</td>
    <td style="padding: 8px; border: 1px solid #333;">Index_b[i]</td>
  </tr>

  <!-- Rows 2-7: first column merged (6 rows in total) -->
  <tr>
    <td rowspan="6" style="padding: 8px; border: 1px solid #333; vertical-align: middle;">Two non-zero elements</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>

  <!-- Rows 8-11: first column merged (4 rows in total) -->
  <tr>
    <td rowspan="4" style="padding: 8px; border: 1px solid #333; vertical-align: middle;">One non-zero element</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>

  <!-- Row 12: not merged -->
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">All zero</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
</table>
This index cube is used for the densification of cube A. Based on the index cube, 2 elements are selected from the 4 elements in CubeA to participate in the computation, as shown in the following figure:

![Dense algorithm illustration](../../../../figures/asc_mmad_sparse_dense_algorithm.png)

## Examples

```cpp
constexpr uint32_t total_length = 128;
__ca__ int8_t a[total_length];
__cb__ int8_t b[total_length];
__cc__ int32_t c[total_length];
uint16_t m = 2;
uint16_t k = 2;
uint16_t n = 1;
asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::ENABLE_KEEP;
bool c_matrix_source = true;
bool c_matrix_init_val = true;
asc_mmad_sparse(c, a, b, m, k, n, unit_flag_mode, c_matrix_source, c_matrix_init_val);
```
