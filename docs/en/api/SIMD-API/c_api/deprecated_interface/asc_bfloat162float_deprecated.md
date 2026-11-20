---
okftype: reference.api
description: "Deprecated implicit position overload that converts elements of a bfloat16_t source vector to float and writes them to the destination vector according to a mask. The version without a suffix reads even indexes, and _v2 reads odd indexes. Use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162float.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The prototype that contains only source, destination, and mask parameters and controls the read position through a suffix is deprecated. Migrate to passing ASC_POSITION_EVEN or ASC_POSITION_ODD. Starting from include/c_api/reg_compute/reg_convert.h:20208, the two declarations carry ASC_DEPRECATED(9.2.0)."
---
# asc_bfloat162float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:17:43.775Z pushedAt=2026-11-18T03:12:16.367Z nl=2 -->

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

**The prototype of this interface that controls the source operand read position through the interface suffix (including only the source operand, destination operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_bfloat162float](../reg_compute/reg_convert/asc_bfloat162float.md) instead.**

Converts each `bfloat16_t` element in `src` that participates in the computation to the `float` type according to `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand to the destination operand type is 1:2, the data of one `VL` size must be divided into two parts when reading, and the position with an even index or an odd index in `src` is selected for reading according to the interface suffix. The pseudocode is as follows:

```python
def asc_bfloat162float(dst, src, mask):
    for i in range(64):
        if mask[2 * i]:
            dst[i] = float(src[2 * i])   # bfloat16 -> float, read the even index.
        else:
            dst[i] = 0

def asc_bfloat162float_v2(dst, src, mask):
    for i in range(64):
        if mask[2 * i + 1]:
            dst[i] = float(src[2 * i + 1])  # bfloat16 -> float, read the odd index.
        else:
            dst[i] = 0
```

### Migration Guide

**Table 1** Correspondence between old and new position parameters

| Old Prototype Position Suffix | New Prototype Position Parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_bfloat162float](../reg_compute/reg_convert/asc_bfloat162float.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_bfloat162float<position_mode>(vector_float& dst,
                                                              vector_bfloat16_t src,
                                                              vector_bool mask)
```

**The placeholders are described as follows:**

- `<position_mode>` indicates the position from which `src` is read. When it is empty, the result reads the even-index positions of `src`; when its value is `_v2`, the result reads the odd-index positions of `src`.

### Prototype Example

```c
// Read the even-index positions of src
__simd_callee__ inline void asc_bfloat162float(vector_float& dst,
                                               vector_bfloat16_t src,
                                               vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | --------- | ---- |
| dst  | Output      | Destination operand (vector data register). |
| src  | Input      | Source operand (vector data register). |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate in the computation. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- Function prototypes that output results through reference parameters return directly when called on non-AIV.
- This API is called within a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned a value in advance through the mask setting API before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect positions of valid elements.
- When the mask bit of `mask` is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
vector_float dst;
vector_bfloat16_t src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_bfloat162float(dst, src, mask);     // Read the positions where the src index is even
asc_bfloat162float_v2(dst, src, mask);  // Read the positions where the src index is odd
```
