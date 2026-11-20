---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Reads even- or odd-indexed elements from the int16 source register based on the interface suffix, performs widening conversion to int32, and writes to the destination register. Use the explicit position parameter overload of asc_int162int32 instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Used to explain vector data registers and mask registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int162int32.md
    kind: replaced_by
    condition: "Use the overload with explicit position parameters to replace the position suffix prototype"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from include/c_api/reg_compute/reg_convert.h:20507, asc_int162int32 and _v2 have no explicit position parameter declaration marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int162int32), replaced by the icposeven/icposodd explicit position overloads; the declaration is still retained in 9.3.0-beta.1, which is deprecated rather than removed."
---
# asc_int162int32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:44:05.572Z pushedAt=2026-11-18T03:12:16.546Z nl=2 -->

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

**The prototype of this interface, which controls the destination operand write position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register), is deprecated. Use the explicit position parameter overload of [asc_int162int32](../reg_compute/reg_convert/asc_int162int32.md) instead.**

Converts the int16_t elements in src to int32_t type and writes the results to dst.

Because the bit-width ratio of the source operand to the destination operand is 1:2, when reading data, one VL-sized data block must be divided into two parts. Depending on the interface, elements at odd indices or even indices are selected as the input data.

## Prototype

```cpp
// Select the even-index elements of src (indices 0, 2, 4, ...) for conversion. The mask is grouped in 2 bits; only when the LSB within a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162int32(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
// Select the odd-index elements of src (indices 1, 3, 5, ...) for conversion. The mask is grouped in 2 bits; only when the LSB within a group is 1 is the int16_t element at the corresponding position selected.
__simd_callee__ inline void asc_int162int32_v2(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means participation in the computation, and 0 means no participation. Elements not selected by the mask are set to zero in the output. |

For detailed descriptions of vector data registers and mask registers, see [reg data type definitions](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

The mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.

## Examples

```cpp
__simd_vf__ inline void int162int32_vf(__ubuf__ int32_t* dst_addr, __ubuf__ int16_t* src_addr, uint32_t count, uint16_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t dst;
    vector_int16_t src;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b16(count);
        asc_loadalign(src, src_addr + i * one_repeat_size);
        asc_int162int32(dst, src, mask);
        asc_storealign(dst_addr + i * one_repeat_size, dst, mask);
    }
}
```
