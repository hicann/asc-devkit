---
okftype: reference.api
description: "Deprecated float-to-bfloat16_t register interface: converts float to bfloat16_t according to the rounding and saturation suffix combination and writes the result to the destination register by even/odd index. The prototype without an explicit position parameter is deprecated; the replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2bfloat16.md
    kind: replaced_by
    condition: "The explicit position parameter overload replaces the deprecated prototype with a position suffix"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "The ctrl register must be configured before using saturation or non-saturation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The entire family of prototypes that use only the source, destination, and mask three parameters with a position suffix to control the write position entered the deprecation period starting from 9.2.0. The include header annotates the rd/rn/rna/ru/rz overloads and their _sat and _sat_v2 overloads one by one with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2bfloat16_<mode>); the CANN_9_2_0 Release Notes deprecation table lists the Reg type conversion position suffix overloads; the interfaces are retained, not removed."
---
# asc_float2bfloat16 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:01:34.459Z pushedAt=2026-11-18T03:12:16.485Z nl=2 -->

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

**The prototypes of this interface that control the destination operand write position through the interface suffix (including only the source operand, destination operand, and mask register parameters) are deprecated. Use the explicit position parameter overload of [asc_float2bfloat16](../reg_compute/reg_convert/asc_float2bfloat16.md) instead.**

Converts each `float` type data in `src` to `bfloat16_t` type according to `mask`, supports multiple rounding modes and saturation modes, and writes the result to `dst`.
Because the bit width ratio of the source operand to the destination operand type is 2:1, when writing data, one `VL`-sized data needs to be divided into two parts. Depending on the interface, the data is written to odd index positions or even index positions. The pseudocode is as follows:

```python
def asc_float2bfloat16_rn(dst, src, mask):
    for i in range(64):
        if mask[i]:
            dst[2 * i] = bfloat16(src[i])   # float -> bfloat16, RINT rounding, write to even index
        else:
            dst[2 * i] = 0
    for i in range(64):
        dst[2 * i + 1] = 0                  # Set odd index to zero

def asc_float2bfloat16_rn_v2(dst, src, mask):
    for i in range(64):
        dst[2 * i] = 0                      # Set even index to zero
        if mask[i]:
            dst[2 * i + 1] = bfloat16(src[i])  # float -> bfloat16, RINT rounding, write to odd index
        else:
            dst[2 * i + 1] = 0
```

For details about rounding modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

### Migration Notes

**Table 1** Correspondence between old and new position parameters

| Old prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_float2bfloat16](../reg_compute/reg_convert/asc_float2bfloat16.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_float2bfloat16_<round_mode><sat_mode><position_mode>(vector_bfloat16_t& dst,
                                                                                     vector_float src,
                                                                                     vector_bool mask)
```

**The placeholders are described as follows:**

- `<round_mode>` indicates the supported rounding mode, which supports `rd` (`FLOOR`), `rn` (`RINT`), `rna` (`ROUND`), `ru` (`CEIL`), and `rz` (`TRUNC`).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates the non-saturation mode; when set to `_sat`, it indicates the saturation mode.
- `<position_mode>` indicates the position where `dst` is written. When empty, the result is written to the even index position of `dst`; when set to `_v2`, the result is written to the odd index position of `dst`.

### Prototype Example

```c
// RINT rounding mode, non-saturation mode, data written to even index positions
__simd_callee__ inline void asc_float2bfloat16_rn(vector_bfloat16_t& dst,
                                                  vector_float src,
                                                  vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description                                                                                                        |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst  | Output      | Destination operand (vector data register).                                                                         |
| src  | Input      | Source operand (vector data register).                                                                              |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The data types of `src` and `dst` must match the function prototype.
- When the `mask` bit is 0, the corresponding element of `dst` is set to 0.
- When the result is written to the odd index positions of `dst`, the even index positions are set to 0; when the result is written to the even index positions, the odd index positions are set to 0.
- When using saturation or non-saturation mode, the `ctrl` register must be configured. For details, see [asc_set_ctrl](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_bfloat16_t dst;
vector_float src;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL);
asc_float2bfloat16_rd(dst, src, mask);    // FLOOR rounding mode, non-saturation mode, converts src to vector_bfloat16_t type and writes it to the even index positions of dst
asc_float2bfloat16_rd_sat(dst, src, mask);    // FLOOR rounding mode, saturation mode, converts src to vector_bfloat16_t type and writes it to the even index positions of dst
asc_float2bfloat16_rd_v2(dst, src, mask);    // FLOOR rounding mode, non-saturation mode, converts src to vector_bfloat16_t type and writes it to the odd index positions of dst
asc_float2bfloat16_rd_sat_v2(dst, src, mask);    // FLOOR rounding mode, saturation mode, convert src to vector_bfloat16_t type and write to the odd index position of dst
```
