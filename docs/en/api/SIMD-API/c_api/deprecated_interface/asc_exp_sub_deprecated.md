---
okftype: reference.api
description: "Deprecated natural exponential difference interface: subtracts two vectors element-wise and writes the natural exponential to dst. The half-input prototype without positional parameters and the asc_exp_sub_v2 prototype have been deprecated, replaced by asc_exp_sub_half2float or the current asc_exp_sub."
tag:
  - roles: [compute]
  - operations: [exponential, subtract]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Vector data register and mask register definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_fused/asc_exp_sub.md
    kind: replaced_by
    condition: "The current interface with the same name takes over the float-input prototype; the half-input uses asc_exp_sub_half2float"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The half-input prototype without positional parameters (asc_exp_sub__Rvf32_vf16_vf16_vb) and the half and float prototypes of asc_exp_sub_v2 entered the deprecation period starting from 9.2.0. The include header marks the involved overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_exp_sub_half2float); the CANN_9_2_0 Release Notes state in the same entry that the half-to-float overloads use asc_exp_sub_half2float instead. The interface is retained and not removed."
---
# asc_exp_sub (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:57:16.915Z pushedAt=2026-11-18T03:12:16.477Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/compute/reg_fused.h"`.

**The `asc_exp_sub` prototype whose input data type is `half` and that does not carry the `src_pos` parameter, as well as the `asc_exp_sub_v2` prototype, have been deprecated. For the new interface, see [asc_exp_sub](../reg_compute/reg_fused/asc_exp_sub.md).**

These interfaces perform element-wise subtraction of `src1` from `src0`, compute the natural exponential function with the difference as the exponent, and write the result to `dst`.

### Migration Notes

**Table 1** Correspondence between legacy and new prototypes

| Legacy prototype | New prototype |
| ------ | ------ |
| `asc_exp_sub(dst, vector_half src0, vector_half src1, mask)` | `asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_EVEN)` |
| `asc_exp_sub_v2(dst, vector_half src0, vector_half src1, mask)` | `asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_ODD)` |
| `asc_exp_sub_v2(dst, vector_float src0, vector_float src1, mask)` | `asc_exp_sub(dst, src0, src1, mask)` |

## Prototype

```c
__simd_callee__ inline void asc_exp_sub(vector_float& dst,
                                        vector_half src0,
                                        vector_half src1,
                                        vector_bool mask)

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst,
                                           vector_half src0,
                                           vector_half src1,
                                           vector_bool mask)

__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst,
                                           vector_float src0,
                                           vector_float src1,
                                           vector_bool mask)

```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst | Output | Destination operand (vector data register). |
| src0 | Input | Source operand 0 (vector data register). |
| src1 | Input | Source operand 1 (vector data register). |
| mask | Input | Mask register, used to indicate the elements that participate in the computation. When the corresponding bit is 1, the element participates in the computation; when it is 0, the element does not participate. Elements that do not participate in the computation are set to 0 in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_float dst;
vector_half src0;
vector_half src1;
vector_bool mask;

// Legacy interface: Read the even-indexed elements of the half source operand.
asc_exp_sub(dst, src0, src1, mask);

// Replacement interface: Explicitly specify the read position of the source operand.
asc_exp_sub_half2float(dst, src0, src1, mask, ASC_POSITION_EVEN);
```
