---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Converts half elements to uint8 with rd, rn, rna, ru, or rz rounding and optional sat saturation mode, and writes to even or odd indexes of the target register based on the suffix. Explicit positional parameter overloads should be used instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body explains the vector data register and mask register accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_half2uint8.md
    kind: replaced_by
    condition: "Use rounding and saturation overloads with explicit positional parameters to replace the position suffix prototype"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The main text explains the rounding mode and saturation/non-saturation mode accordingly"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "The ctrl register must be configured before using saturation or non-saturation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The asc_half2uint8_rd/_rd_sat (starting at lines 21066-21092) and the _sat and _v2 position suffix declarations of rn/rna/ru/rz in include/c_api/reg_compute/reg_convert.h are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2uint8_rd / asc_half2uint8_rn, etc.), replaced by explicit positional parameter overloads; the declarations are still retained in 9.3.0-beta.1, which is deprecation rather than removal."
---
# asc_half2uint8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:35:35.028Z pushedAt=2026-11-18T03:12:16.535Z nl=2 -->

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

**The prototype that controls the target operand write position through an interface suffix (including only the three parameters: source operand, target operand, and mask register) is deprecated. Use the explicit positional parameter overload of [asc_half2uint8](../reg_compute/reg_convert/asc_half2uint8.md) instead.**

Converts each `half` type element in `src` that participates in the computation to `uint8_t` type according to `mask`, and writes the result to `dst`. Because the bit width ratio of the source operand to the target operand is 2:1, the data of one `VL` size must be split into two parts when writing. Based on the interface suffix, the data is written to either the even-indexed or odd-indexed positions of `dst`. The pseudocode is as follows:

```python
def asc_half2uint8_rd(dst, src, mask):
    for i in range(128):
        if mask[i]:
            dst[2 * i] = uint8(src[i])   # half -> uint8, FLOOR rounding, write to even index
        else:
            dst[2 * i] = 0
    for i in range(128):
        dst[2 * i + 1] = 0               # Set odd index to zero

def asc_half2uint8_rd_v2(dst, src, mask):
    for i in range(128):
        dst[2 * i] = 0                   # Set even index to zero
        if mask[i]:
            dst[2 * i + 1] = uint8(src[i])  # half -> uint8, FLOOR rounding, write to odd index
        else:
            dst[2 * i + 1] = 0
```

For detailed descriptions of rounding modes and saturation/non-saturation modes, see [Rounding Modes](../reg_compute/reg_convert/rounding_mode.md).

### Migration Notes

**Table 1** Correspondence between old and new positional parameters

| Old Prototype Position Suffix | New Prototype Positional Parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_half2uint8](../reg_compute/reg_convert/asc_half2uint8.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_half2uint8_<round_mode><sat_mode><position_mode>(vector_uint8_t& dst,
                                                                                 vector_half src,
                                                                                 vector_bool mask)
```

**The placeholders are described as follows:**

- `<round_mode>` indicates the supported rounding modes, including `rd` (`FLOOR`), `rn` (`RINT`), `rna` (`ROUND`), `ru` (`CEIL`), and `rz` (`TRUNC`).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.
- `<position_mode>` indicates the position where `dst` is written. When empty, the result is written to the even-indexed positions of `dst`; when set to `_v2`, the result is written to the odd-indexed positions of `dst`.

### Prototype Example

```c
// FLOOR rounding mode, non-saturation mode, write to even-indexed positions of dst
__simd_callee__ inline void asc_half2uint8_rd(vector_uint8_t& dst,
                                              vector_half src,
                                              vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst  | Output      | Target operand (vector data register). |
| src  | Input      | Source operand (vector data register). |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit in `mask` corresponding to an element is 1, that element participates in the computation; when it is 0, that element does not participate in the computation. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- Function prototypes that output results through reference parameters return directly when called on non-AIV.
- This interface is called within a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned in advance through the mask setting interface before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect valid element positions.
- When data is written to positions with odd indices in the target operand, the values at even-index positions are set to zero. The reverse is also true.
- When using saturation or non-saturation mode, the `ctrl` register must be configured. For details, see [asc_set_ctrl](../spr/asc_set_ctrl.md).
- When the `mask` bit is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
vector_uint8_t dst;
vector_half src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_half2uint8_rd(dst, src, mask);        // FLOOR rounding mode, non-saturation mode, write to even-index positions of dst.
asc_half2uint8_rd_sat(dst, src, mask);    // FLOOR rounding mode, saturation mode, write to the even index of dst.
asc_half2uint8_rd_v2(dst, src, mask);     // FLOOR rounding mode, non-saturation mode, write to the odd index of dst.
asc_half2uint8_rd_sat_v2(dst, src, mask); // FLOOR rounding mode, saturation mode, write to the odd index of dst.
```
