---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Reads the even- or odd-indexed elements of the half source register according to the interface suffix, and converts them to int32 with rd, rn, rna, ru, or rz rounding and writes them to the destination register. Use the overload with an explicit position parameter instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text describes the vector data register and mask register accordingly."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_half2int32.md
    kind: replaced_by
    condition: "Use the rounding overload with an explicit position parameter to replace the position suffix prototype."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The body text explains the meaning of each rounding mode based on this"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The declarations without a position suffix in include/c_api/reg_compute/reg_convert.h, such as asc_half2int32_rd__Rvi32_vf16_vb (lines 18811-18818) and _v2__Rvi32_vf16_vb (starting at lines 21406-21428), are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2int32_rd / asc_half2int32_rn), replaced by overloads with ASC_POSITION_EVEN/ODD; the declarations are still retained in 9.3.0-beta.1, meaning they are deprecated rather than removed."
---
# asc_half2int32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:29:09.197Z pushedAt=2026-11-18T03:12:16.525Z nl=2 -->

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

**The prototype that controls the source operand read position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_half2int32](../reg_compute/reg_convert/asc_half2int32.md) instead.**

Converts each `half` element in `src` that participates in the computation to the `int32_t` type according to `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand type to the destination operand type is 1:2, the data of one `VL` size must be divided into two parts when reading, and the interface suffix determines whether to read the even-indexed or odd-indexed positions of `src`. The pseudocode is as follows:

```python
def asc_half2int32_rd(dst, src, mask):
    for i in range(64):
        if mask[2 * i]:
            dst[i] = int32(src[2 * i])   # half -> int32, FLOOR rounding, read even indices
        else:
            dst[i] = 0

def asc_half2int32_rd_v2(dst, src, mask):
    for i in range(64):
        if mask[2 * i + 1]:
            dst[i] = int32(src[2 * i + 1])  # half -> int32, FLOOR rounding, read odd indices
        else:
            dst[i] = 0
```

For details about rounding modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

### Migration Description

**Table 1** Correspondence between legacy and new position parameters

| Legacy prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_half2int32](../reg_compute/reg_convert/asc_half2int32.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_half2int32_<round_mode><position_mode>(vector_int32_t& dst,
                                                                       vector_half src,
                                                                       vector_bool mask)
```

**The placeholders are described as follows:**

- `<round_mode>` indicates the supported rounding mode, which can be `rd` (`FLOOR`), `rn` (`RINT`), `rna` (`ROUND`), `ru` (`CEIL`), or `rz` (`TRUNC`).
- `<position_mode>` indicates the position read from `src`. When empty, the result reads the even-indexed positions of `src`; when set to `_v2`, the result reads the odd-indexed positions of `src`.

### Prototype Example

```c
// FLOOR rounding mode, read the even-indexed positions of src
__simd_callee__ inline void asc_half2int32_rd(vector_int32_t& dst,
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

- Function prototypes that output results through reference parameters return directly when called on non-AIV.
- This interface is called within a Vector Function (a function marked with `__simd_vf__`).
- `mask` must be assigned a value in advance through the mask setting interface before being passed in. The content of an unassigned mask register is undefined, which may cause incorrect valid element positions.
- When a mask bit in `mask` is 0, the corresponding element in `dst` is set to 0.

## Examples

```cpp
vector_int32_t dst;
vector_half src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_half2int32_rd(dst, src, mask);     // FLOOR rounding mode, read the even-indexed positions of src
asc_half2int32_rd_v2(dst, src, mask);  // FLOOR rounding mode, read the odd-indexed positions of src
```
