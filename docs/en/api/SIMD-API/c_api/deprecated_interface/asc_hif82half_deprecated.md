---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Converts the even or odd indexed elements of the hifloat8 source to half under the half bit-width ratio and writes them to the destination register. Use the explicit position parameter overload of asc_hif82half instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text explains the vector data register and mask register accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_hif82half.md
    kind: replaced_by
    condition: "Use the overload with explicit position parameters to replace the position suffix prototype"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from include/c_api/reg_compute/reg_convert.h:20598, asc_hif82half and asc_hif82half_v2 have no explicit position parameter declaration marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_hif82half), replaced by the icposeven/icposodd explicit position overloads; the declaration is still retained in 9.3.0-beta.1, which is deprecated rather than removed."
---
# asc_hif82half (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:40:12.884Z pushedAt=2026-11-18T03:12:16.540Z nl=2 -->

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

**The prototype of this interface that controls the destination operand write position through the interface suffix (including only the source operand, destination operand, and mask register parameters) is deprecated. Use the explicit position parameter overload of [asc_hif82half](../reg_compute/reg_convert/asc_hif82half.md) instead.**

Converts the elements of the hifloat8_t data type in src to the half data type, and writes the results to dst.

Because the bit-width ratio between the source operand and destination operand types is 1:2, when reading data, one VL-sized data block must be divided into two parts. Depending on the interface, the input data at odd-indexed positions or even-indexed positions is selected.

## Prototype

```cpp
// Select the even-indexed elements of src (index 0, 2, 4, ...) for conversion. When each bit of mask is 1, the hifloat8_t element at the corresponding position is selected.
__simd_callee__ inline void asc_hif82half(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
// Select the odd-indexed elements of src (index 1, 3, 5, ...) for conversion. When each bit of mask is 1, the hifloat8_t element at the corresponding position is selected.
__simd_callee__ inline void asc_hif82half_v2(vector_half& dst, vector_hifloat8_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.

## Examples

```cpp
__simd_vf__ inline void hif82half_vf(__ubuf__ half* dst_addr, __ubuf__ hifloat8_t* src_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_half dst;
    vector_hifloat8_t src;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b8(count);
        asc_loadalign(src, src_addr + i * one_repeat_size);
        asc_hif82half(dst, src, mask);
        asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
    }
}
```
