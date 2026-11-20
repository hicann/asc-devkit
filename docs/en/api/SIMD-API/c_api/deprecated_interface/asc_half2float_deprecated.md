---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Selects even or odd index elements of the half source register based on the interface suffix and converts them to float written to the destination register according to the mask. Use the explicit position parameter overload of asc_half2float instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text explains vector data registers and mask registers based on this."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_half2float.md
    kind: replaced_by
    condition: "Use the overload with the ASC_POSITION_EVEN or ASC_POSITION_ODD explicit position parameter to replace the position suffix prototype."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The position suffix prototypes asc_half2float__Rvf32_vf16_vb (lines 19637-19648) and asc_half2float_v2__Rvf32_vf16_vb (lines 22750-22761) in include/c_api/reg_compute/reg_convert.h are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2float), replaced by overloads with an explicit position parameter; the explicit position versions (asc_half2float__Rvf32_vf16_vb_icposeven/icposodd) are retained, so this is a deprecation rather than a removal."
---
# asc_half2float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:24:17.351Z pushedAt=2026-11-18T03:12:16.521Z nl=2 -->

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

**The prototype that controls the source operand read position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_half2float](../reg_compute/reg_convert/asc_half2float.md) instead.**

Converts each `half` type element in `src` that participates in the computation to `float` type according to `mask`, and writes the result to `dst`. Because the bit width ratio of the source operand to the destination operand type is 1:2, when reading data, a `VL`-sized data block must be divided into two parts. Based on the interface suffix, select to read the even-indexed or odd-indexed positions of `src`. The pseudocode is as follows:

```python
def asc_half2float(dst, src, mask):
    for i in range(64):
        if mask[2 * i]:
            dst[i] = float(src[2 * i])   # half -> float, read even index
        else:
            dst[i] = 0

def asc_half2float_v2(dst, src, mask):
    for i in range(64):
        if mask[2 * i + 1]:
            dst[i] = float(src[2 * i + 1])  # half -> float, read odd index
        else:
            dst[i] = 0
```

### Migration Description

**Table 1** Correspondence between old and new position parameters

| Old prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_half2float](../reg_compute/reg_convert/asc_half2float.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_half2float<position_mode>(vector_float& dst,
                                                          vector_half src,
                                                          vector_bool mask)
```

**The placeholders are described as follows:**

- `<position_mode>` indicates the position from which `src` is read. When it is empty, the result reads the even-index positions of `src`; when its value is `_v2`, the result reads the odd-index positions of `src`.

### Prototype Example

```c
// Read the even-index positions of src
__simd_callee__ inline void asc_half2float(vector_float& dst,
                                           vector_half src,
                                           vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst  | Output      | Destination operand (vector data register). |
| src  | Input      | Source operand (vector data register). |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit in `mask` corresponding to an element is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The function prototype that outputs the result through a reference parameter returns directly when called on a non-AIV.
- This interface is called inside a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned in advance through the mask setting interface before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect positions of valid elements.
- When the mask bit of `mask` is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
vector_float dst;
vector_half src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_half2float(dst, src, mask);     // Read the positions where the src index is even
asc_half2float_v2(dst, src, mask);  // Read the positions where the src index is odd
```
