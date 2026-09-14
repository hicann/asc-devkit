# asc_half2hif8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:00:00.133Z pushedAt=2026-09-05T12:14:28.845Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Converts the half type to the hifloat8_t type and supports multiple rounding modes.

- ROUND rounding mode: round half up.
- HYBRID rounding mode: a stochastic rounding mode used specifically when outputting hifloat8_t data.

## Prototype

```cpp
// ROUND rounding mode, non-saturation mode, data is written to even-indexed positions.
__simd_callee__ inline void asc_half2hif8_rna(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturation mode, data is written to even-indexed positions.
__simd_callee__ inline void asc_half2hif8_rna_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data is written to odd-indexed positions.
__simd_callee__ inline void asc_half2hif8_rna_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturation mode, data is written to odd-indexed positions.
__simd_callee__ inline void asc_half2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data is written to even-indexed positions.
__simd_callee__ inline void asc_half2hif8_rh(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data is written to even-indexed positions.
__simd_callee__ inline void asc_half2hif8_rh_sat(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data is written to odd-indexed positions.
__simd_callee__ inline void asc_half2hif8_rh_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data is written to odd-indexed positions.
__simd_callee__ inline void asc_half2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_half src, vector_bool mask)
```

## Parameters

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- Non-saturation mode:
  - When the input data exceeds the range of the output type, the corresponding signed inf value of the output type is returned.
  - When the input data is +/-inf, +/-inf is returned.
  - When the input data is nan, nan is returned.
- Saturation mode:
  - When the input data exceeds the range of the output type, the corresponding maximum value of the output type is returned.
  - When the input data is +/-inf, the corresponding extreme value of the output type is returned.
  - When the input data is nan, 0 is returned.
- When saturation mode and non-saturation mode are enabled, the **ctrl** register must be configured. For details about the **ctrl** register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_half src;
vector_hifloat8_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the UB memory space address externally input.
asc_half2hif8_rna(dst, src, mask);
```
