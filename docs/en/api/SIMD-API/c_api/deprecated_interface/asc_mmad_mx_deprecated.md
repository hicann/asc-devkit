---
okftype: reference.api
description: "Deprecated MX matrix multiply-accumulate interface. Performs matrix multiply-accumulate with left and right quantization coefficient matrices on the AIC and supports C matrix initialization configuration. Available only on Ascend 950PR&950DT. The replacement is the strongly typed asc_mmad_mx used with asc_sync."
tag:
  - roles: [compute]
  - operations: [matrix_multiply_accumulate]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_mx.md
    kind: replaced_by
    condition: "Strongly typed interface that replaces the uint8_t unit_flag prototype on this page"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md
    kind: requires
    condition: "Before calling, use this interface to move the ScaleA matrix into the L0A_MX Buffer"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md
    kind: requires
    condition: "Before the call, use this API to move the ScaleB matrix into the L0B_MX Buffer"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: related
    condition: "Configure saturation or non-saturation handling of inf/nan through CTRL[48]"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Synchronization method that replaces the original synchronization API"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md
    kind: related
    condition: "When the K-axis is accumulated in blocks and the number of fractals is less than 10, PIPE_M must be synchronized between adjacent multiply-accumulate operations"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The asc_mmad_mx and asc_mmad_mx_sync prototypes (16 in total) whose input parameter unit_flag is of type uint8_t are marked as deprecated in include/c_api/cube_compute/cube_compute.h with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_mmad_mx(...)). The alternative is to use asc_mmad_mx with asc_unit_flag_mode plus asc_sync. All prototypes on this page fall within the deprecation scope, and no upper limit on availability is imposed."
---
# asc_mmad_mx (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:19:05.594Z pushedAt=2026-11-18T03:12:16.607Z nl=2 -->

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

The header file paths are: `"c_api/cube_compute/cube_compute.h"` and `"c_api/composite/cube_compute_composite.h"`.

**The `asc_mmad_mx` and `asc_mmad_mx_sync` interfaces whose input parameter `unit_flag` is of type `uint8_t` are deprecated. Use the [asc_mmad_mx](../cube_compute/mmad_compute/asc_mmad_mx.md) interface whose input parameter `unit_flag_mode` is of type `asc_unit_flag_mode` instead. The synchronization function of the original `asc_mmad_mx_sync` interface can be implemented through the new `asc_mmad_mx` interface and `asc_sync()`. For details, see [asc_sync](../sync/intra_core_sync/asc_sync.md).**

This interface is used to perform matrix multiply-accumulate computation with quantization coefficients (MmadMx). That is, both the left matrix A and the right matrix B have corresponding quantization coefficient matrices, namely the left quantization coefficient matrix ScaleA and the right quantization coefficient matrix ScaleB. In the MmadMx scenario, the left quantization coefficient matrix is multiplied by the left matrix, the right quantization coefficient matrix is multiplied by the right matrix, and then matrix multiplication is performed on the results of the two products. The calculation formula is as follows:

$$
C_{M \times N} = \left(ScaleA_{M \times (K/32)} \otimes A_{M \times K}\right) \times \left(ScaleB_{(K/32) \times N} \otimes B_{K \times N}\right) + C_{M \times N}
$$

Here, $\otimes$ denotes broadcast multiplication. Every 32 consecutive elements of matrices A and B along the K dimension share one quantization coefficient in the corresponding quantization coefficient matrices ScaleA and ScaleB. By configuring the parameters of this interface, the C matrix can be initialized to an all-zero matrix, a matrix in the L0C Buffer, or a Bias matrix.

This interface does not pass the Scale matrix addresses separately. Instead, based on the start addresses of matrices A and B, it reads the corresponding Scale matrices from the L0A_MX Buffer and L0B_MX Buffer according to a fixed address mapping relationship. The information of each matrix is described in the following table:

**Table 1** Matrix information description

| Matrix | Storage Location | Shape (rows × columns) | Data Format | Fractal Size (rows × columns) |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Nz | 16×K0 |
| ScaleA | L0A_MX Buffer | M×(K / 32) | Zz | 16×2 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| ScaleB | L0B_MX Buffer | (K / 32)×N | Nn | 2×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias (used for C matrix initialization) | BiasTable Buffer | 1×N, used to initialize the C matrix by broadcasting and replicating M rows | ND | - |

When the matrix data type is `fp8_e5m2_t` or `fp8_e4m3fn_t`, `K0 = 32`. When the matrix data type is `fp4x2_e2m1_t` or `fp4x2_e1m2_t`, `K0 = 64`.

This interface is a cube computation interface and takes effect only on the AIC.

## Prototype

```c
// Placeholder form
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
                                   __ca__ <a_dtype>* a_matrix,
                                   __cb__ <b_dtype>* b_matrix,
                                   uint16_t left_height,
                                   uint16_t n_dim,
                                   uint16_t right_width,
                                   uint8_t unit_flag,
                                   bool disable_gemv,
                                   bool c_matrix_source,
                                   bool c_matrix_init_val)

__aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix,
                                        __ca__ <a_dtype>* a_matrix,
                                        __cb__ <b_dtype>* b_matrix,
                                        uint16_t left_height,
                                        uint16_t n_dim,
                                        uint16_t right_width,
                                        uint8_t unit_flag,
                                        bool disable_gemv,
                                        bool c_matrix_source,
                                        bool c_matrix_init_val)
```

### Data Types Supported by dtype

The value combinations of `a_dtype` and `b_dtype` are shown in the following table:

**Table 2** Supported data type combinations<a id="asc_mmad_mx_data_type"></a>

| a_dtype | b_dtype | ScaleA/ScaleB Data Type | Result Matrix C Data Type | Bias Data Type |
| --- | --- | --- | --- | --- |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e1m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `fp8_e8m0_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `fp8_e8m0_t` | `float` | `float` |

### Prototype Example

```c
__aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix,
                                   __ca__ fp4x2_e1m2_t* a_matrix,
                                   __cb__ fp4x2_e2m1_t* b_matrix,
                                   uint16_t left_height,
                                   uint16_t n_dim,
                                   uint16_t right_width,
                                   uint8_t unit_flag,
                                   bool disable_gemv,
                                   bool c_matrix_source,
                                   bool c_matrix_init_val)
```

## Parameters

**Table 3** Parameter Description<a id="asc_mmad_mx_param_table"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| c_matrix | Output | Destination operand, the start address of the result matrix C in the L0C Buffer. The data type is `float`, and it must be aligned to 1024 bytes. |
| a_matrix | Input | Source operand, the start address of the left matrix A in the L0A Buffer. The data type is determined by the interface overload. For details, see [Table 2](#asc_mmad_mx_data_type).<br>When the matrix data type is `fp4x2_e2m1_t` or `fp4x2_e1m2_t`, it must be aligned to 512 bytes. When the matrix data type is `fp8_e5m2_t` or `fp8_e4m3fn_t`, it must be aligned to 1024 bytes.<br>The start address of the ScaleA matrix in the L0A_MX Buffer is the start address of `a_matrix` divided by 16. |
| b_matrix | Input | Source operand, the start address of the right matrix B in the L0B Buffer. The data type is determined by the interface overload. For details, see [Table 2](#asc_mmad_mx_data_type).<br>When the matrix data type is `fp4x2_e2m1_t` or `fp4x2_e1m2_t`, it must be aligned to 512 bytes. When the matrix data type is `fp8_e5m2_t` or `fp8_e4m3fn_t`, it must be aligned to 1024 bytes.<br>The start address of the ScaleB matrix in the L0B_MX Buffer is the start address of `b_matrix` divided by 16. |
| left_height | Input | The M dimension size of the left matrix A and the result matrix C, in elements. The value range is [0, 4095]. |
| n_dim | Input | The K dimension size of the left matrix A and the right matrix B, in elements. The value range is [0, 4095], and it must be a multiple of 64. |
| right_width | Input | The N dimension size of the right matrix B and the result matrix C, in elements. The value range is [0, 4095]. |
| unit_flag | Input | Used to control fine-grained parallelism between the matrix multiply-accumulate instruction and the matrix move-out instruction. After UnitFlag is enabled, the hardware moves out the computation result each time it finishes computing one fractal. The value description is as follows:<br>&nbsp;&nbsp;&bull; 0: UnitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2: UnitFlag is enabled, and the hardware does not change the unit flag after executing the instruction.<br>&nbsp;&nbsp;&bull; 3: UnitFlag is enabled, and the hardware changes the unit flag after executing the instruction.<br>The matrix multiply-accumulate instruction and the corresponding matrix move-out instruction must both enable or both disable UnitFlag. After UnitFlag is enabled, no synchronization instruction needs to be inserted between the instructions. |
| disable_gemv | Input | When M is 1, configures whether to disable the GEMV mode.<br>&nbsp;&nbsp;&bull; false: The GEMV mode is enabled.<br>&nbsp;&nbsp;&bull; true: The GEMV mode is disabled.<br>When M is not 1, this parameter does not take effect. |
| c_matrix_source | Input | When the parameter `c_matrix_init_val` is false, configures the source of the initial value of matrix C.<br>&nbsp;&nbsp;&bull; false: The initial value of matrix C comes from the L0C Buffer.<br>&nbsp;&nbsp;&bull; true: The initial value of matrix C comes from the BiasTable Buffer. |
| c_matrix_init_val | Input | Configures whether to set the initial value of matrix C to 0.<br>&nbsp;&nbsp;&bull; true: The initial value of matrix C is set to 0, and the parameter `c_matrix_source` does not take effect.<br>&nbsp;&nbsp;&bull; false: The zeroing operation is not performed, and the initial value of matrix C is configured by the parameter `c_matrix_source`. |

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- This interface takes effect only on the AIC. When called on the AIV, it returns directly.
- When any one of `left_height`, `n_dim`, and `right_width` is 0, the interface is treated as a NOP (no operation).
- Before calling this API, use [asc_copy_l12l0a_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md) and [asc_copy_l12l0b_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md) to move the ScaleA and ScaleB matrices into the L0A_MX Buffer and L0B_MX Buffer, respectively. The data type of the ScaleA and ScaleB matrices must be `fp8_e8m0_t`, and each Scale element in the K direction corresponds to 32 input matrix elements.

- Memory usage constraint description:

  - The L0C Buffer size is 256 KB, the L0A Buffer and L0B Buffer sizes are both 64 KB, the L0A_MX Buffer and L0B_MX Buffer sizes are both 4 KB, and the BiasTable Buffer size is 4 KB. The start address and occupied space of a matrix must not exceed the range of the corresponding Buffer.
  - The start address of each matrix must meet the alignment requirements in [Table 3](#asc_mmad_mx_param_table). For other address constraints of the operands, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
  - The start address of the ScaleA matrix in the L0A_MX Buffer is the start address of `a_matrix` divided by 16, and the start address of the ScaleB matrix in the L0B_MX Buffer is the start address of `b_matrix` divided by 16. When moving in the Scale matrices, use destination addresses that match matrix A and matrix B.
  - `n_dim` must be a multiple of 64. If the original K is not a multiple of 64, pad K upward to a multiple of 64 and set the matrix A and matrix B data in the padded region to 0. The `n_dim` passed to the API, the matrix storage space, and the K dimension size of the Scale matrices must all be calculated based on the padded K.
  - The M and N dimensions of the matrix storage space must be padded upward to multiples of 16, respectively, while `left_height` and `right_width` still pass the valid M and N values of the matrix. The padded part is invalid data and does not participate in the calculation of the valid region of the result matrix.
  - The Scale matrices must cover all data required for this matrix multiply-accumulate operation. The M dimension of ScaleA must be padded upward to a multiple of 16, and its K dimension size is K / 32. The K dimension size of ScaleB is K / 32, and its N dimension must be padded upward to a multiple of 16. The corresponding start addresses and occupied space must not exceed the range of the MX Buffer.
  - When M is 1 and `disable_gemv` is false, the GEMV mode is enabled by default. In this case, matrix A is read from the L0A Buffer in ND format, matrix A must be arranged in ND format, and its start address must still meet the alignment requirements in [Table 3](#asc_mmad_mx_param_table).

- Synchronization constraint description:

  For the scenario where the input matrix is computed in blocks along the K axis and the results are accumulated into the same L0C Buffer block, when `(left_height / 16) * (right_width / 16) < 10`, call [asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md) between two adjacent Matrix Multiply-Accumulate instructions, and set the input parameter `pipe` to `PIPE_M`.

- UnitFlag Constraint Description:

  - When UnitFlag is enabled, the Matrix Multiply-Accumulate instruction and the corresponding Matrix Move-Out instruction must enable UnitFlag at the same time. When you want the same L0C Buffer memory space to be continuously operated only by multiple Matrix Multiply-Accumulate instructions or multiple Matrix Move-Out instructions, set the unitFlag value of the first n-1 instructions to 2 to maintain the continuous occupation state of the operated memory space, and set the last instruction to 3 to release the occupied state.
  - When UnitFlag is enabled, the matrix computation direction must be consistent with the Matrix Move-Out read order. When the Matrix Move-Out instruction enables Nz2ND in-pass format conversion, or does not perform in-pass format conversion but enables B8/B4 quantization and triggers the Channel Merge function, call [asc_set_mmad_direction_n](../cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_n.md); in other scenarios, call [asc_set_mmad_direction_m](../cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_m.md).
  - When UnitFlag is enabled, it is recommended that the computation data volume of Matrix Multiply-Accumulate be consistent with the data volume of Matrix Move-Out. Inconsistency between the two may cause execution exceptions. When you need to clear the residual state generated by UnitFlag, call [asc_set_l0c2gm_config (Deprecated)](asc_set_l0c2gm_config_deprecated.md), set `enable_unit_flag` to true, set the unit flag bits of all memory blocks in the L0C Buffer to 0, and disable UnitFlag.

- Special Value/Boundary Value Constraint Description:

  When the floating-point input or output contains inf/nan, you can configure the CTRL\[48\] bit of the CTRL register through the [asc_set_ctrl](../spr/asc_set_ctrl.md) interface to control the mode during computation:

  - When set to 0, the saturation mode is used, where inf output is saturated to ±MAX and nan output is saturated to 0;
  - When set to 1, the non-saturation mode is used, where inf/nan keeps the original output.

  Note that nan input should be avoided, otherwise an execution error may occur.
