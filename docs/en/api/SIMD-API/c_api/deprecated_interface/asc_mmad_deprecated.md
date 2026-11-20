---
okftype: reference.api
description: "Deprecated Matrix Multiply-Accumulate interface that performs the matrix computation C = A × B + C on the AIC and supports configuration of the Result Matrix initialization source. The old prototype passes parameters via uint8_t unit_flag and feat_offset. The replacement is the strongly typed asc_mmad used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [matrix_multiply_accumulate]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: replaced_by
    condition: "The strongly typed interface that replaces the uint8_t unit_flag and feat_offset prototype on this page"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "See that page for the Storage Unit description and operand address constraints"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: related
    condition: "Configure saturated or non-saturated handling of inf/nan through CTRL[48]"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Synchronization method that replaces the original sync interface"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_pipe.md
    kind: related
    condition: "When K-axis block accumulation is used and the fractal count is less than 10, PIPE_M must be synchronized between adjacent multiply-accumulate operations"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added a set of Matrix Multiply-Accumulate prototypes for NPU Architecture 3510 (Ascend 950PR&950DT), supporting same-precision and mixed-precision combinations of int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, and float, and switched to the disable_gemv parameter; based on line 50 of the CANN_9_1_0 Release Notes and the npu_arch_3510/cube_compute_impl.h declaration that first appeared in upstream/9.1.0 (this architecture had no such set of prototypes in 9.0.0 or earlier)."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The asc_mmad and asc_mmad_sync prototypes whose input parameter unit_flag is of type uint8_t (including the feat_offset/smask_offset/is_weight_offset forms of 2201) are marked deprecated in include/c_api/cube_compute/cube_compute.h with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_mmad(...)); the replacement is to use asc_mmad with asc_unit_flag_mode plus asc_sync. The strongly-typed prototypes (unitflag formal parameter) do not carry this marker, so this is a partial prototype deprecation, and the page does not impose an upper limit on availability."
---
# asc_mmad (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:18:18.051Z pushedAt=2026-11-18T03:12:16.609Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

The header file paths are: `"c_api/cube_compute/cube_compute.h"` and `"c_api/composite/cube_compute_composite.h"`.

**The `asc_mmad` interface whose input parameter `unit_flag` is of type `uint8_t`, and the `asc_mmad_sync` interface, are deprecated. Use the [asc_mmad](../cube_compute/mmad_compute/asc_mmad.md) interface whose input parameter `unit_flag_mode` is of type `asc_unit_flag_mode` instead. Implement the original synchronization functionality through the new interface and `asc_sync()`. For details, see [asc_sync](../sync/intra_core_sync/asc_sync.md).**

<!-- npu="A3,910b" id18 -->
For the following product models:

<!-- npu="A3" id22 -->
Atlas A3 products
<!-- end id22 -->
<!-- npu="910b" id23 -->
Atlas A2 products
<!-- end id23 -->
**The `asc_mmad` and `asc_mmad_s4` interfaces whose input parameters include `feat_offset`, and all `asc_mmad_sync` and `asc_mmad_s4_sync` interfaces, are deprecated. For interfaces with the `feat_offset` parameter, use the corresponding interfaces that do not have this parameter and use `asc_unit_flag_mode` instead.**
<!-- end id18 -->

This interface is the core Matrix Multiply-Accumulate (Mmad) computation interface of Ascend C for Ascend AI chips. It is designed specifically for high-performance operator development, encapsulating the Matrix Multiply-Accumulate computation capability of Ascend NPU hardware. It is widely used in the development of neural network layers (such as fully connected layers and convolutional layers) and numerical computation operators. Its calculation formula is as follows:

$$
C_{M \times N} = A_{M \times K} \times B_{K \times N} + C_{M \times N}
$$

where A, B, and C are the Left Matrix, Right Matrix, and Result Matrix, respectively. The C matrix can be initialized, by configuring the parameters of this interface, to an all-zero matrix, a matrix in the L0C Buffer, or a Bias matrix. The information of each matrix is described in the following table:

<!-- npu="950" id8 -->
**Table** Matrix information description ([NPU Architecture Version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Matrix | Storage Location | Shape (rows × columns) | Data Format | Fractal Size (rows × columns) |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Nz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias (used for C matrix initialization) | BiasTable Buffer | 1×N; when used, M rows are replicated by broadcast to initialize the C matrix | ND | - |

In the table, the value of K0 is `32B / sizeof(dtype)`, where `dtype` is the data type of the matrix.
<!-- end id8 -->

<!-- npu="A3,910b" id19 -->
**Table** Matrix information description ([NPU Architecture Version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Matrix | Storage Location | Shape | Data Format | Fractal Size |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M×K | Zz | 16×K0 |
| B | L0B Buffer | K×N | Zn | K0×16 |
| C | L0C Buffer | M×N | Nz | 16×16 |
| Bias (used for C matrix initialization) | BiasTable Buffer | 1×N, broadcast to M rows during use to initialize the C matrix | ND | - |

The value of K0 is `32B / sizeof(dtype)`, where `dtype` is the data type of the matrix. When the data type is `int4b_t`, `K0 = 64`.
<!-- end id19 -->

This interface is a matrix computation interface and takes effect only on the AIC.

## Prototype

<!-- npu="950" id12 -->

### Function Prototype ([NPU Architecture Version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

```c
// Placeholder form
__aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                __ca__ <a_dtype>* a_matrix,
                                __cb__ <b_dtype>* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t unit_flag,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix,
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

#### Data Types Supported by dtype

The value combinations of `c_dtype`, `a_dtype`, and `b_dtype` are shown in the following table:

**Table** Supported data type combinations ([NPU Architecture Version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="asc_mmad_data_type"></a>

| a_dtype | b_dtype | c_dtype | Bias Data Type |
| --- | --- | --- | --- |
| `int8_t` | `int8_t` | `int32_t` | `int32_t` |
| `hifloat8_t` | `hifloat8_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` | `float` |
| `half` | `half` | `float` | `float` |
| `bfloat16_t` | `bfloat16_t` | `float` | `float` |
| `float` | `float` | `float` | `float` |

#### Prototype Example

```c
__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t unit_flag,
                                bool disable_gemv,
                                bool c_matrix_source,
                                bool c_matrix_init_val)
```
<!-- end id12 -->

<!-- npu="A3,910b" id20 -->

### Function Prototype ([NPU Architecture Version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

- `asc_mmad` and `asc_mmad_sync`:

  ```c
  // Placeholder form
  __aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                  __ca__ <a_dtype>* a_matrix,
                                  __cb__ <b_dtype>* b_matrix,
                                  uint16_t left_height,
                                  uint16_t n_dim,
                                  uint16_t right_width,
                                  uint8_t unit_flag,
                                  bool k_direction_align,
                                  bool c_matrix_source,
                                  bool c_matrix_init_val)

  __aicore__ inline void asc_mmad(__cc__ <c_dtype>* c_matrix,
                                  __ca__ <a_dtype>* a_matrix,
                                  __cb__ <b_dtype>* b_matrix,
                                  uint16_t left_height,
                                  uint16_t n_dim,
                                  uint16_t right_width,
                                  uint8_t feat_offset,
                                  uint8_t smask_offset,
                                  uint8_t unit_flag,
                                  bool k_direction_align,
                                  bool is_weight_offset,
                                  bool c_matrix_source,
                                  bool c_matrix_init_val)

  __aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix,
                                       __ca__ <a_dtype>* a_matrix,
                                       __cb__ <b_dtype>* b_matrix,
                                       uint16_t left_height,
                                       uint16_t n_dim,
                                       uint16_t right_width,
                                       uint8_t unit_flag,
                                       bool k_direction_align,
                                       bool c_matrix_source,
                                       bool c_matrix_init_val)

  __aicore__ inline void asc_mmad_sync(__cc__ <c_dtype>* c_matrix,
                                       __ca__ <a_dtype>* a_matrix,
                                       __cb__ <b_dtype>* b_matrix,
                                       uint16_t left_height,
                                       uint16_t n_dim,
                                       uint16_t right_width,
                                       uint8_t feat_offset,
                                       uint8_t smask_offset,
                                       uint8_t unit_flag,
                                       bool k_direction_align,
                                       bool is_weight_offset,
                                       bool c_matrix_source,
                                       bool c_matrix_init_val)
  ```

#### Data Types Supported by dtype

`c_dtype`, `a_dtype`, and `b_dtype` support the following combinations: `float/bfloat16_t/bfloat16_t`, `float/half/half`, `float/float/float`, and `int32_t/int8_t/int8_t`.

- `asc_mmad_s4` and `asc_mmad_s4_sync`:

  ```c
  __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix,
                                     __ca__ int4b_t* a_matrix,
                                     __cb__ int4b_t* b_matrix,
                                     uint16_t left_height,
                                     uint16_t n_dim,
                                     uint16_t right_width,
                                     uint8_t unit_flag,
                                     bool k_direction_align,
                                     bool c_matrix_source,
                                     bool c_matrix_init_val)

  __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix,
                                     __ca__ int4b_t* a_matrix,
                                     __cb__ int4b_t* b_matrix,
                                     uint16_t left_height,
                                     uint16_t n_dim,
                                     uint16_t right_width,
                                     uint8_t feat_offset,
                                     uint8_t smask_offset,
                                     uint8_t unit_flag,
                                     bool k_direction_align,
                                     bool is_weight_offset,
                                     bool c_matrix_source,
                                     bool c_matrix_init_val)

  __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix,
                                          __ca__ int4b_t* a_matrix,
                                          __cb__ int4b_t* b_matrix,
                                          uint16_t left_height,
                                          uint16_t n_dim,
                                          uint16_t right_width,
                                          uint8_t unit_flag,
                                          bool k_direction_align,
                                          bool c_matrix_source,
                                          bool c_matrix_init_val)

  __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix,
                                          __ca__ int4b_t* a_matrix,
                                          __cb__ int4b_t* b_matrix,
                                          uint16_t left_height,
                                          uint16_t n_dim,
                                          uint16_t right_width,
                                          uint8_t feat_offset,
                                          uint8_t smask_offset,
                                          uint8_t unit_flag,
                                          bool k_direction_align,
                                          bool is_weight_offset,
                                          bool c_matrix_source,
                                          bool c_matrix_init_val)
  ```

#### Prototype Example

```c
__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t unit_flag,
                                bool k_direction_align,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

__aicore__ inline void asc_mmad(__cc__ float* c_matrix,
                                __ca__ bfloat16_t* a_matrix,
                                __cb__ bfloat16_t* b_matrix,
                                uint16_t left_height,
                                uint16_t n_dim,
                                uint16_t right_width,
                                uint8_t feat_offset,
                                uint8_t smask_offset,
                                uint8_t unit_flag,
                                bool k_direction_align,
                                bool is_weight_offset,
                                bool c_matrix_source,
                                bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix,
                                     __ca__ bfloat16_t* a_matrix,
                                     __cb__ bfloat16_t* b_matrix,
                                     uint16_t left_height,
                                     uint16_t n_dim,
                                     uint16_t right_width,
                                     uint8_t unit_flag,
                                     bool k_direction_align,
                                     bool c_matrix_source,
                                     bool c_matrix_init_val)

__aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix,
                                     __ca__ bfloat16_t* a_matrix,
                                     __cb__ bfloat16_t* b_matrix,
                                     uint16_t left_height,
                                     uint16_t n_dim,
                                     uint16_t right_width,
                                     uint8_t feat_offset,
                                     uint8_t smask_offset,
                                     uint8_t unit_flag,
                                     bool k_direction_align,
                                     bool is_weight_offset,
                                     bool c_matrix_source,
                                     bool c_matrix_init_val)
```
<!-- end id20 -->

## Parameters

<!-- npu="950" id14 -->
**Table** Parameter Description ([NPU Architecture Version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="asc_mmad_param_table"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| c_matrix | Output | Destination operand, the start address of the result matrix C in the L0C Buffer, which must be 1024-byte aligned. The data type is determined by the interface overload. For details, see [Supported Data Type Combinations](#asc_mmad_data_type). |
| a_matrix | Input | Source operand, the start address of the left matrix A in the L0A Buffer, which must be 512-byte aligned. The data type is determined by the interface overload. For details, see [Supported Data Type Combinations](#asc_mmad_data_type). |
| b_matrix | Input | Source operand, the start address of the right matrix B in the L0B Buffer, which must be 512-byte aligned. The data type is determined by the interface overload. For details, see [Supported Data Type Combinations](#asc_mmad_data_type). |
| left_height | Input | M dimension size of the left matrix A and the result matrix C, in elements, with a value range of [0, 4095]. |
| n_dim | Input | K dimension size of the left matrix A and the right matrix B, in elements, with a value range of [0, 4095]. |
| right_width | Input | N dimension size of the right matrix B and the result matrix C, in elements, with a value range of [0, 4095]. |
| unit_flag | Input | Used to control fine-grained parallelism between the matrix multiply-accumulate instruction and the matrix move-out instruction. When UnitFlag is enabled, the hardware moves out the computation result each time it finishes computing one fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0: UnitFlag is disabled.<br>&nbsp;&nbsp;&bull; 2: UnitFlag is enabled, and the hardware does not change the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3: UnitFlag is enabled, and the hardware changes the unit flag bit after executing the instruction.<br>The matrix multiply-accumulate instruction and the corresponding matrix move-out instruction must both enable or both disable UnitFlag. When UnitFlag is enabled, no synchronization instruction needs to be inserted between the instructions. |
| disable_gemv | Input | When M is 1, configures whether to disable GEMV mode.<br>&nbsp;&nbsp;&bull; false: GEMV mode is enabled.<br>&nbsp;&nbsp;&bull; true: GEMV mode is disabled.<br>When M is not 1, this parameter does not take effect. |
| c_matrix_source | Input | When the parameter `c_matrix_init_val` is false, configures the source of the initial value of matrix C.<br>&nbsp;&nbsp;&bull; false: The initial value of matrix C comes from the L0C Buffer.<br>&nbsp;&nbsp;&bull; true: The initial value of matrix C comes from the BiasTable Buffer. |
| c_matrix_init_val | Input | Configures whether to set the initial value of matrix C to 0.<br>&nbsp;&nbsp;&bull; true: The initial value of matrix C is set to 0, and the parameter `c_matrix_source` does not take effect.<br>&nbsp;&nbsp;&bull; false: No zeroing operation is performed, and the initial value of matrix C is configured by the parameter `c_matrix_source`. |

<!-- end id14 -->

<!-- npu="A3,910b" id21 -->
**Table** Parameter Description ([NPU Architecture Version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Parameter | Input/Output | Description |
| --- | --- | --- |
| c_matrix | Output | Start address of the result matrix C in the L0C Buffer, which must be 1024-byte aligned. |
| a_matrix | Input | Start address of the left matrix A in the L0A Buffer, which must be 512-byte aligned. |
| b_matrix | Input | Start address of the right matrix B in the L0B Buffer, which must be 512-byte aligned. |
| left_height | Input | M dimension size of the left matrix A and the result matrix C, with a value range of [0, 4095]. |
| n_dim | Input | K dimension size of the left matrix A and the right matrix B, with a value range of [0, 4095]. |
| right_width | Input | N dimension size of the right matrix B and the result matrix C, with a value range of [0, 4095]. |
| feat_offset | Input | Invalid parameter. Users do not need to pay attention to it and can pass 0. |
| smask_offset | Input | Invalid parameter. Users do not need to pay attention to it and can pass 0. |
| unit_flag | Input | UnitFlag control parameter. 0 indicates disabled; 2 indicates enabled and the unit flag bit is maintained after execution; 3 indicates enabled and the unit flag bit is updated after execution. |
| k_direction_align | Input | Controls the alignment in the K direction when the float data type is used. false indicates alignment to `ceil(n_dim / 8) * 8`, and true indicates alignment to `ceil(n_dim / 16) * 16`. |
| is_weight_offset | Input | Invalid parameter. Users do not need to pay attention to it and can pass false. |
| c_matrix_source | Input | When `c_matrix_init_val` is false, configures whether the initial value of matrix C comes from the BiasTable Buffer. |
| c_matrix_init_val | Input | Configures whether to set the initial value of matrix C to 0. |

<!-- end id21 -->

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- This API takes effect only on the AIC. Calling it on the AIV returns directly.
- When any of `left_height`, `n_dim`, and `right_width` is 0, the API is treated as a NOP (no operation).

- Memory usage constraints:
  <!-- npu="950" id10 -->
  - For Ascend 950PR&950DT products:

      - The L0C Buffer size is 256KB, and both the L0A Buffer and L0B Buffer sizes are 64KB. The BiasTable Buffer size is 4KB. The start address and occupied space of a matrix must not exceed the range of the corresponding buffer.
      - The start address of each matrix must meet the alignment requirements in [Parameter Description](#asc_mmad_param_table). For other address constraints of operands, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
      - When applying for matrix storage space, use the values padded to the fractal size: M and N are each padded up to a multiple of 16, and K is padded up to a multiple of K0, where K0 is `32B / sizeof(dtype)` and `dtype` is the data type of the matrix. `left_height`, `n_dim`, and `right_width` still pass in the valid M, K, and N values of the matrix. The padded part is invalid data and does not participate in the calculation of the valid region of the result matrix.
      - When M is 1 and `disable_gemv` is false, GEMV mode is enabled. In this case, matrix A is read from the L0A Buffer in ND format, matrix A must be arranged in ND format, and the start address must still be 512-byte aligned.
  <!-- end id10 -->

  <!-- npu="A3,910b" id11 -->
  - For the following product models:

 	  <!-- npu="A3" id16 -->
    Atlas A3 products
 	  <!-- end id16 -->
 	  <!-- npu="910b" id17 -->
 	  Atlas A2 products
 	  <!-- end id17 -->
 	  The L0C Buffer size is 128KB, and both the L0A Buffer and L0B Buffer sizes are 64KB. The BiasTable Buffer size is 1KB. The start address and occupied space of a matrix must not exceed the range of the corresponding buffer.
  <!-- end id11 -->

- Synchronization constraints:

  For the scenario where the input matrix is computed in blocks along the K axis and the results are accumulated into the same L0C Buffer block, when `(left_height / 16) * (right_width / 16) < 10`, you need to call [asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md) between two adjacent matrix multiply-accumulate instructions and set the input parameter `pipe` to `PIPE_M`.

- UnitFlag constraints:

  - When UnitFlag is enabled, the matrix multiply-accumulate instruction and the corresponding matrix move-out instruction must enable UnitFlag at the same time. When you want the same L0C Buffer memory space to be continuously operated only by multiple matrix multiply-accumulate instructions or multiple matrix move-out instructions, you need to set the unitFlag value of the first n-1 instructions to 2 to maintain the continuous occupation state of the operated memory space, and set the last instruction to 3 to release the occupied state.
  - When UnitFlag is enabled, the matrix computation direction must be consistent with the matrix move-out read order. When the matrix move-out instruction enables Nz2ND in-path format conversion, or when in-path format conversion is not performed but B8/B4 quantization is enabled and the Channel Merge function is triggered, call [asc_set_mmad_direction_n](../cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_n.md); in other scenarios, call [asc_set_mmad_direction_m](../cube_compute/mmad_compute_aux_config/asc_set_mmad_direction_m.md).
  - When UnitFlag is enabled, it is recommended that the computation data volume of the matrix multiply-accumulate be consistent with the data volume of the matrix move-out. Inconsistency between the two may cause execution exceptions. When you need to clear the residual state generated by UnitFlag, you can call [asc_set_l0c2gm_config (deprecated)](asc_set_l0c2gm_config_deprecated.md) and set `enable_unit_flag` to true to set the unit flag bits of all memory blocks in the L0C Buffer to 0 and disable UnitFlag.

- Special value/boundary value constraints:

  When the floating-point input or output contains inf/nan, you can configure the CTRL\[48\] bit of the CTRL register through the [asc_set_ctrl](../spr/asc_set_ctrl.md) interface to control the computation mode:

  - When set to 0, the saturation mode is used, where inf output is saturated to ±MAX and nan output is saturated to 0;
  - When set to 1, the non-saturation mode is used, where inf/nan keeps the original output.

  Note that nan input should be avoided, otherwise execution errors may occur; integer types support only saturation mode.
