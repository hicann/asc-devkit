---
okftype: reference.api
description: "Deprecated implicit location overload that converts a bfloat16_t source vector to int32_t by mask, covering the five rounding modes rn, rna, rd, ru, rz combined with saturation, non-saturation, and even/odd index. Use the explicit location parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, saturation_mode, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162int32.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "When using saturation or non-saturation mode, the ctrl register must be configured first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The overloads that control the read position via the interface suffix are deprecated, including the combination of the _v2 position suffix and the _sat saturation suffix; the 20 declarations in include/c_api/reg_compute/reg_convert.h:20299-20416 all carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_bfloat162int32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:20:13.201Z pushedAt=2026-11-18T03:12:16.370Z nl=2 -->

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

**The prototype that controls the source operand read location through the interface suffix (including only the source operand, destination operand, and mask register parameters) is deprecated. Use the explicit location parameter overload of [asc_bfloat162int32](../reg_compute/reg_convert/asc_bfloat162int32.md) instead.**

Converts each `bfloat16_t` element in `src` that participates in the computation to the `int32_t` type according to `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand type to the destination operand type is 1:2, the data of one `VL` size must be divided into two parts when reading. Based on the interface suffix, select to read the even-indexed or odd-indexed positions of `src`. The pseudocode is as follows:

```python
def asc_bfloat162int32_rn(dst, src, mask):
    for i in range(64):
        if mask[i]:
            dst[i] = int32(src[2 * i])   # bfloat16 -> int32, RINT rounding, read even indices
        else:
            dst[i] = 0

def asc_bfloat162int32_rn_v2(dst, src, mask):
    for i in range(64):
        if mask[i]:
            dst[i] = int32(src[2 * i + 1])  # bfloat16 -> int32, RINT rounding, read odd indices
        else:
            dst[i] = 0
```

For details about rounding modes and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

### Migration Notes

**Table 1** Correspondence between old and new position parameters

| Old prototype location suffix | New prototype position parameter |
| --- | --- |
| No location suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

When migrating, remove the location suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_bfloat162int32](../reg_compute/reg_convert/asc_bfloat162int32.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_bfloat162int32_<round_mode><sat_mode><position_mode>(vector_int32_t& dst,
                                                                                     vector_bfloat16_t src,
                                                                                     vector_bool mask)
```

**The placeholders are described as follows:**

- `<round_mode>` indicates the supported rounding mode, which supports `rd` (`FLOOR`), `rn` (`RINT`), `rna` (`ROUND`), `ru` (`CEIL`), and `rz` (`TRUNC`).
- `<sat_mode>` indicates the saturation/non-saturation mode. When empty, it indicates non-saturation mode; when set to `_sat`, it indicates saturation mode.
- `<position_mode>` indicates the position from which `src` is read. When empty, the result reads the even-indexed positions of `src`; when set to `_v2`, the result reads the odd-indexed positions of `src`.

### Prototype Example

```c
// RINT rounding mode, non-saturation mode, read the even-indexed positions of src
__simd_callee__ inline void asc_bfloat162int32_rn(vector_int32_t& dst,
                                                  vector_bfloat16_t src,
                                                  vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst  | Output      | Destination operand (vector data register). |
| src  | Input      | Source operand (vector data register). |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The function prototype that outputs the result through a reference parameter returns directly when called on non-AIV.
- This API is called within a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned a value in advance through the mask setting API before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect valid element positions.
- When using saturation or non-saturation mode, the `ctrl` register needs to be configured. For details, see [asc_set_ctrl](../spr/asc_set_ctrl.md).
- When the `mask` bit is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
vector_int32_t dst;
vector_bfloat16_t src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_bfloat162int32_rd(dst, src, mask);         // FLOOR rounding mode, non-saturation mode, read the positions with even src indices
asc_bfloat162int32_rd_sat(dst, src, mask);     // FLOOR rounding mode, saturation mode, read the positions with even src indices
asc_bfloat162int32_rd_v2(dst, src, mask);      // FLOOR rounding mode, non-saturating mode, read the position where the src index is odd
asc_bfloat162int32_rd_sat_v2(dst, src, mask);  // FLOOR rounding mode, saturating mode, read the position where the src index is odd
```
