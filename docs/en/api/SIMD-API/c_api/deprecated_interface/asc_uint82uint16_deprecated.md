---
okftype: reference.api
description: "Deprecated prototype description for uint8_t to uint16_t vector conversion. The old position suffix form is replaced by an overload of the same name with an explicit position parameter tag. The source and destination bit-width ratio is 1:2, read by even or odd index."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint16.md
    kind: replaced_by
    condition: "Migrate to the explicit position parameter overload"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_uint82uint16__Rvu16_vu8_vb_icposeven"
    scope_basis: resolved
    note: "Added an explicit position parameter overload (fourth parameter asc_position_mode::EVEN), replacing the prototype without a position suffix."
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_uint82uint16__Rvu16_vu8_vb_icposodd"
    scope_basis: resolved
    note: "Added an explicit position parameter overload (asc_position_mode::ODD), replacing the _v2 position suffix prototype."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The header file marks the prototypes without a position suffix and the _v2 prototype with ASC_DEPRECATED(9.2.0); the replacement is to remove the position suffix and pass the position constant as the fourth parameter. Deprecated only, not removed, with no availability limit set."
---
# asc_uint82uint16 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:21:09.652Z pushedAt=2026-11-18T03:12:16.701Z nl=2 -->

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

**The prototype that controls the source operand read position through an interface suffix (including only the three parameters: source operand, destination operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_uint82uint16](../reg_compute/reg_convert/asc_uint82uint16.md) instead.**

Convert each `uint8_t` element in the source operand `src` to `uint16_t` according to `mask`, and write the result to `dst`. Because the bit-width ratio between the source operand and destination operand types is 1:2, when reading data, one `VL`-sized data block must be split into two parts, and the position parameter selects whether the input data index is an odd position or an even position. Refer to the following pseudocode:

```python
# The position check is only used to illustrate the interface semantics. The actual position parameter is a compile-time constant.
# VL indicates the vector data register bit width, which is 256 bytes.
def asc_uint82uint16(dst, src, mask, src_pos):
    offset = 0 if src_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 2):
        src_index = 2 * i + offset
        dst[i] = uint16(src[src_index]) if mask[src_index] else 0
```

### Migration Description

**Table 1** Correspondence between old and new position parameters

| Old prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_uint82uint16](../reg_compute/reg_convert/asc_uint82uint16.md).

## Prototype

```c
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)

__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                                                                               |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst  | Output      | Destination operand (vector data register).                                                                                     |
| src  | Input      | Source operand (vector data register).                                                                                       |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |
| src_pos | Input | Position selection tag (compiler tag type), of type `std::integral_constant<asc_position_mode, asc_position_mode::EVEN>` or `std::integral_constant<asc_position_mode, asc_position_mode::ODD>`. When `ASC_POSITION_EVEN` is used, the even-indexed positions of the source operand are selected for reading; when `ASC_POSITION_ODD` is used, the odd-indexed positions of the source operand are selected for reading. |

For detailed descriptions of vector data registers and mask registers, see [reg data type definitions](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The data types of `src` and `dst` must match the function prototype.
- When the `mask` bit is 0, the corresponding element of `dst` is set to 0.

## Examples

```c
vector_uint8_t src;
vector_half dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the externally input Unified Buffer (UB) memory space.
asc_uint82uint16(dst, src, mask);
```
