---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Truncates or saturation-converts int16 source register elements to uint8 and writes them to the 0th or 1st position within each group of the destination register according to the suffix. Use the overload with explicit position parameters instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text explains vector data registers and mask registers accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int162uint8.md
    kind: replaced_by
    condition: "Use the saturation/non-saturation overload with explicit position parameters to replace the position suffix prototype"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The body text explains saturation and non-saturation modes based on this"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from include/c_api/reg_compute/reg_convert.h:21248 and 21261, the declarations of asc_int162uint8, asc_int162uint8_sat, and their _v2 are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int162uint8 / asc_int162uint8_sat), replaced by overloads with explicit position parameters; in 9.3.0-beta.1 the declarations are still retained, being deprecated rather than removed."
---
# asc_int162uint8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:45:35.091Z pushedAt=2026-11-18T03:12:16.550Z nl=2 -->

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

**The prototype of this interface that controls the destination operand write position through the interface suffix (including only the source operand, destination operand, and mask register parameters) is deprecated. Use the explicit position parameter overload of [asc_int162uint8](../reg_compute/reg_convert/asc_int162uint8.md) instead.**

Converts the int16_t elements in src to uint8_t type and writes the results to dst. Both non-saturating and saturating modes are supported.

Because the bit-width ratio of the source operand to the destination operand is 2:1, when writing data, one VL-sized data must be divided into two parts. Depending on the interface, data is written to positions with odd indexes or even indexes.

## Prototype

```cpp
// Non-saturating mode. The result is written to position 0 in each group of dst, directly clipped. mask is grouped in 2 bits. Only when the LSB in a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162uint8(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
// Non-saturating mode. The result is written to position 1 in each group of dst, directly clipped. mask is grouped in 2 bits. Only when the LSB in a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162uint8_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
// Saturating mode. Values outside the range [0, 255] are clamped to the boundary. The result is written to position 0 in each group of dst. mask is grouped in 2 bits. Only when the LSB in a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162uint8_sat(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
// Saturating mode. Values outside the range [0, 255] are clamped to the boundary. The result is written to position 1 in each group of dst. mask is grouped in 2 bits. Only when the LSB in a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162uint8_sat_v2(vector_uint8_t& dst, vector_int16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

mask controls whether the source operand participates in the computation. For elements of the source operand that do not participate in the computation, the corresponding positions in the output are set to zero.

## Examples

```cpp
__simd_vf__ inline void int162uint8_vf(__ubuf__ uint8_t* dst_addr, __ubuf__ int16_t* src_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t dst;
    vector_int16_t src;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign(src, src_addr + i * one_repeat_size);
        asc_int162uint8(dst, src, mask);
        asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
    }
}
```
