---
okftype: reference.api
description: "Deprecated Reg multiply-by-scalar interface that multiplies a float source vector by a scalar value, then rounds the result to half using ROUND rounding and writes it to the destination register. It includes two implicit position overloads, the unsuffixed and _v2 variants, and its replacement is asc_mul_scalar_float2half_rn."
tag:
  - roles: [compute]
  - operations: [conversion, multiply]
  - traits: [rounding_mode, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_fused/asc_mul_scalar_float2half_rn.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_muls and asc_muls_v2 are deprecated; use asc_mul_scalar_float2half_rn instead. Both declarations starting from include/c_api/reg_compute/compute/reg_fused.h:581 carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table lists this entry at row 126."
---
# asc_muls (deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:28:19.434Z pushedAt=2026-11-18T03:12:16.625Z nl=2 -->

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

> This API is deprecated. Use [asc_mul_scalar_float2half_rn](../reg_compute/reg_fused/asc_mul_scalar_float2half_rn.md) instead.

The header file path is `"c_api/reg_compute/compute/reg_fused.h"`.

**This API is deprecated.**

Performs multiplication on the source operands src and value, converts the result to the half type using the ROUND rounding mode for data type conversion, and writes the result to the destination operand dst according to mask. The calculation formula is as follows:

$$
dst_i = round\_to\_half(src_i \times value)
$$

## Prototype

```cpp
// Write data to positions with even indexes.
__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask)
// Write data to positions with odd indexes.
__simd_callee__ inline void asc_muls_v2(vector_half& dst, vector_float src, float value, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| value | Input | Source operand (scalar). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the calculation. A value of 1 at the corresponding position means the element participates in the calculation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_float src;
vector_half dst;
float value = 1.0f;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr);  // src_addr is the Unified Buffer (UB) memory space address from external input.
asc_muls(dst, src, value, mask);
```
