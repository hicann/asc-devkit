---
okftype: reference.api
description: "Deprecated Mmad mode switching interface. After invocation, it enables FP8 mode and disables HiF8 mode, so that FP8 data in L0A and L0B is no longer converted to hifloat8_t before participating in matrix multiply-accumulate. The replacement is asc_disable_hif8."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_disable_hif8.md
    kind: replaced_by
    condition: "Disable HiF8 mode with a semantically explicit parameterless interface"
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hif8.md
    kind: constrained_by
    condition: "The two have opposite effects and do not take effect simultaneously"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_disable_hif8); the CANN_9_2_0 Release Notes deprecation table entry also states to use asc_disable_hif8 instead; the interface is still retained in 3510 and has not been removed."
---
# asc_enable_fp8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:51:43.456Z pushedAt=2026-11-18T03:12:16.470Z nl=2 -->

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

**The `asc_enable_fp8` API is deprecated. Use [asc_disable_hif8](../cube_compute/mmad_compute_aux_config/asc_disable_hif8.md) instead.**

The header file path is `"c_api/cube_compute/cube_compute.h"`.

Used to enable FP8 mode for Mmad computation. This API disables HiF8 mode (the opposite of [asc_enable_hif8](../cube_compute/mmad_compute_aux_config/asc_enable_hif8.md)). After this mode is enabled, FP8 data in the L0A Buffer/L0B Buffer is no longer converted to hifloat8_t type data before participating in Mmad computation.

## Prototype

```cpp
__aicore__ inline void asc_enable_fp8()
```

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The opposite of [asc_enable_hif8](../cube_compute/mmad_compute_aux_config/asc_enable_hif8.md); the two do not take effect at the same time.
- Must be called before [asc_mmad](../cube_compute/mmad_compute/asc_mmad.md) is executed.

## Examples

```cpp
constexpr uint64_t total_length = 128;
__cc__ float c_matrix[total_length];
__ca__ fp8_e4m3fn_t a_matrix[total_length];
__cb__ fp8_e4m3fn_t b_matrix[total_length];

uint16_t left_height = 16;  // M
uint16_t n_dim = 16;        // K
uint16_t right_width = 16;  // N

// Call before asc_mmad: participate in matrix computation in FP8 format (non-HiF8 conversion path)
asc_enable_fp8();

asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::DISABLE;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
asc_mmad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
         unit_flag_mode, disable_gemv, c_matrix_source, c_matrix_init_val);
asc_sync();
```
