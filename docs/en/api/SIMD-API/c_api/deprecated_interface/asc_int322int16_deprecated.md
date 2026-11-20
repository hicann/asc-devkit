---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Converts int32 source register elements to int16 in optional sat saturation mode and writes them to the even or odd indices of the destination register according to the suffix. Use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text describes vector data registers and mask registers accordingly."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int322int16.md
    kind: replaced_by
    condition: "Use the saturation/non-saturation overload with an explicit position parameter to replace the position suffix prototype."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The main text describes saturation and non-saturation modes accordingly"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from include/c_api/reg_compute/reg_convert.h:19922 and 19935, asc_int322int16, asc_int322int16_sat, and their _v2 declarations are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int322int16 / asc_int322int16_sat), replaced by overloads with explicit position parameters; the declarations are still retained in 9.3.0-beta.1, which is deprecation rather than removal."
---
# asc_int322int16 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:47:08.240Z pushedAt=2026-11-18T03:12:16.553Z nl=2 -->

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

**The prototype that controls the destination operand write position through an interface suffix (including only the source operand, destination operand, and mask register parameters) is deprecated. Use the explicit position parameter overload of [asc_int322int16](../reg_compute/reg_convert/asc_int322int16.md) instead.**

Converts each `int32_t` element in `src` to an `int16_t` element according to `mask`, supports saturation/non-saturation modes, and writes the result to `dst`. Because the bit-width ratio of the source operand to the destination operand is 2:1, the data of one `VL` size must be divided into two parts when writing, and the data is written to either odd-indexed or even-indexed positions depending on the interface. The reference pseudocode is as follows:

```python
def asc_int322int16(dst, src, mask):
    for i in range(64):
        if mask[i]:
            dst[2 * i] = int16(src[i])   # int32 -> int16, write to even index
        else:
            dst[2 * i] = 0
    for i in range(64):
        dst[2 * i + 1] = 0               # Set odd index to zero

def asc_int322int16_v2(dst, src, mask):
    for i in range(64):
        dst[2 * i] = 0                   # Set even index to zero
        if mask[i]:
            dst[2 * i + 1] = int16(src[i])  # int32 -> int16, write to odd index
        else:
            dst[2 * i + 1] = 0
```

For details about the rounding mode and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

### Migration Instructions

**Table 1** Correspondence between old and new position parameters

| Old prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_int322int16](../reg_compute/reg_convert/asc_int322int16.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_int322int16<sat_mode><position_mode>(vector_int16_t& dst,
                                                                     vector_int32_t src,
                                                                     vector_bool mask)
```

**The placeholders are described as follows:**

- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.
- `<position_mode>` indicates the position where `dst` is written. When empty, the result is written to the even-indexed positions of `dst`; when set to `_v2`, the result is written to the odd-indexed positions of `dst`.

### Prototype Example

```c
// Non-saturation mode, data is written to even-indexed positions
__simd_callee__ inline void asc_int322int16(vector_int16_t& dst,
                                            vector_int32_t src,
                                            vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description                                                                                                        |
| -------------- | ------------ | ------------------------------------------------------------------------------------------------------------------ |
| dst  | Output      | Destination operand (vector data register).                                                                         |
| src  | Input       | Source operand (vector data register).                                                                              |
| mask | Input       | Mask register, used to control whether each element participates in the computation. When the bit in `mask` corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The data types of `src` and `dst` must match the function prototype.
- When the mask bit of `mask` is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
__simd_vf__ inline void int322int16_vf(__ubuf__ int32_t* src_addr, __ubuf__ int16_t* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_int32_t src;
    vector_int16_t dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b32(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_int322int16(dst, src, mask);
        asc_storealign_pack_postupdate(dst_addr, dst, one_block_size, mask);
    }
}
```
