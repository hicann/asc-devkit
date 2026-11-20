---
okftype: reference.api
description: "Deprecated Reg prototype description without an explicit position parameter. Converts the hifloat8 source register to float and writes it to the destination register by selecting the index position of a quarter VL according to the _v2 to _v4 suffix. Use the overload with an explicit position parameter instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text describes the vector data register and the mask register accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_hif82float.md
    kind: replaced_by
    condition: "Use the overload with an explicit position parameter to replace the position-suffix prototype"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_hif82float and the _v2/_v3/_v4 declarations without explicit position parameters in include/c_api/reg_compute/reg_convert.h (starting at lines 21027-21066) are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_hif82float), replaced by explicit position overloads such as icposqfirst/icposqsecond; the declarations are still retained in 9.3.0-beta.1, which is deprecation rather than removal."
---
# asc_hif82float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:38:14.761Z pushedAt=2026-11-18T03:12:16.538Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/reg_convert.h"`.

**The prototype of this interface without an explicit position parameter is deprecated. Use the overload with an explicit position parameter of [asc_hif82float](../reg_compute/reg_convert/asc_hif82float.md) instead.**

Converts hifloat8_t data to float type.

Because the bit-width ratio between the source operand and the destination operand is 1:4, when reading data, one VL of data must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the different interface.

## Prototype

```cpp
// Select the position of index 0.
__simd_callee__ inline void asc_hif82float(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
// Select the position of index 1.
__simd_callee__ inline void asc_hif82float_v2(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
// Select the position of index 2.
__simd_callee__ inline void asc_hif82float_v3(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
// Select the position of index 3.
__simd_callee__ inline void asc_hif82float_v4(vector_float& dst, vector_hifloat8_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst        | Output    | Destination operand (vector data register). |
| src        | Input    | Source operand (vector data register). |
| mask       | Input    | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_hifloat8_t src;
vector_float dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the Unified Buffer (UB) memory space address input from outside.
asc_hif82float(dst, src, mask);
```
