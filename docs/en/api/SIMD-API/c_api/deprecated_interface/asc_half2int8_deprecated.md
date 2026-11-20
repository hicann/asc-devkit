---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description: converts half elements to int8 using rd, rn, rna, ru, or rz rounding and optional sat saturation mode, and writes to the even or odd index of the destination register according to the suffix. Use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text explains vector data registers and mask registers based on this."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_half2int8.md
    kind: replaced_by
    condition: "Use the rounding and saturation overload with explicit position parameters to replace the position suffix prototype."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The main text describes the rounding mode and the saturating/non-saturating mode accordingly"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "The ctrl register must be configured before using the saturating or non-saturating mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The asc_half2int8_rd/_rd_sat (starting at lines 20026-20052) and the _sat and _v2 position suffix declarations of rn/rna/ru/rz in include/c_api/reg_compute/reg_convert.h are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2int8_rd / asc_half2int8_rn, etc.), replaced by explicit position parameter overloads; the declarations are still retained in 9.3.0-beta.1, which is deprecation rather than removal."
---
# asc_half2int8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:33:33.261Z pushedAt=2026-11-18T03:12:16.533Z nl=2 -->

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

**The prototype that controls the target operand write position through an interface suffix (including only the three parameters: source operand, target operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_half2int8](../reg_compute/reg_convert/asc_half2int8.md) instead.**

Converts each `half` type element in `src` that participates in the computation to `int8_t` type according to `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand to the target operand type is 2:1, the data of one `VL` size must be divided into two parts when writing, and the interface suffix selects whether to write to the even-indexed or odd-indexed positions of `dst`. The pseudocode is as follows:

```python
def asc_half2int8_rd(dst, src, mask):
    for i in range(128):
        if mask[i]:
            dst[2 * i] = int8(src[i])   # half -> int8, FLOOR rounding, write to even index
        else:
            dst[2 * i] = 0
    for i in range(128):
        dst[2 * i + 1] = 0              # Set odd index to zero

def asc_half2int8_rd_v2(dst, src, mask):
    for i in range(128):
        dst[2 * i] = 0                  # Set even index to zero
        if mask[i]:
            dst[2 * i + 1] = int8(src[i])  # half -> int8, FLOOR rounding, write to odd index
        else:
            dst[2 * i + 1] = 0
```

For detailed descriptions of rounding mode and saturation/non-saturating mode, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

### Migration Guide

**Table 1** Mapping between old and new position parameters

| Old Prototype Position Suffix | New Prototype Position Parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_half2int8](../reg_compute/reg_convert/asc_half2int8.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_half2int8_<round_mode><sat_mode><position_mode>(vector_int8_t& dst,
                                                                                vector_half src,
                                                                                vector_bool mask)
```

**The placeholders are described as follows:**

- `<round_mode>` indicates the supported rounding mode, which can be `rd` (`FLOOR`), `rn` (`RINT`), `rna` (`ROUND`), `ru` (`CEIL`), or `rz` (`TRUNC`).
- `<sat_mode>` indicates the saturation/non-saturating mode. When empty, it indicates the non-saturating mode; when set to `_sat`, it indicates the saturation mode.
- `<position_mode>` indicates the position where `dst` is written. When empty, the result is written to the even-indexed positions of `dst`; when set to `_v2`, the result is written to the odd-indexed positions of `dst`.

### Prototype Example

```c
// FLOOR rounding mode, non-saturating mode, write to even-indexed positions of dst
__simd_callee__ inline void asc_half2int8_rd(vector_int8_t& dst,
                                             vector_half src,
                                             vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst  | Output      | Target operand (vector data register). |
| src  | Input      | Source operand (vector data register). |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate in the computation. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The function prototype that outputs the result through a reference parameter returns directly when called on a non-AIV.
- This API is called within a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned a value in advance through the mask setting API before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect valid element positions.
- When data is written to positions with odd indices in the target operand, the values at positions with even indices are set to zero. The reverse is also true.
- When using the saturating or non-saturating mode, the `ctrl` register must be configured. For details, see [asc_set_ctrl](../spr/asc_set_ctrl.md).
- When the `mask` bit is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
vector_int8_t dst;
vector_half src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_half2int8_rd(dst, src, mask);        // FLOOR rounding mode, non-saturating mode, write to positions with even indices in dst
asc_half2int8_rd_sat(dst, src, mask);    // FLOOR rounding mode, saturation mode, write to the even-indexed positions of dst
asc_half2int8_rd_v2(dst, src, mask);     // FLOOR rounding mode, non-saturating mode, write to the odd-indexed positions of dst
asc_half2int8_rd_sat_v2(dst, src, mask); // FLOOR rounding mode, saturation mode, write to the odd-indexed positions of dst
```
