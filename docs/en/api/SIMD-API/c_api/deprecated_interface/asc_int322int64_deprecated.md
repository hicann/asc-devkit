---
okftype: reference.api
description: "Deprecated Reg position suffix prototype description. Reads the even- or odd-indexed elements of the int32 source register based on the interface suffix, widens and converts them to int64, and writes them to the destination register. Use the explicit position parameter overload of asc_int322int64 instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The main text describes the vector data register and mask register based on this."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int322int64.md
    kind: replaced_by
    condition: "Use the overload with explicit position parameters to replace the position suffix prototype."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from include/c_api/reg_compute/reg_convert.h:21508, asc_int322int64 and _v2 without explicit position parameter declaration are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_int322int64), replaced by icposeven/icposodd explicit position overloads; the declaration is still retained in 9.3.0-beta.1, which is deprecated rather than removed."
---
# asc_int322int64 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:48:56.306Z pushedAt=2026-11-18T03:12:16.557Z nl=2 -->

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

**The prototype of this interface, which controls the source operand read position through the interface suffix (including only the source operand, destination operand, and mask register parameters), is deprecated. Use the explicit position parameter overload of [asc_int322int64](../reg_compute/reg_convert/asc_int322int64.md) instead.**

Converts each `int32_t` element in `src` to an `int64_t` element based on `mask`, and writes the result to `dst`. Because the bit-width ratio of the source operand to the destination operand is 1:2, when reading data, one `VL`-sized data block must be divided into two parts. Depending on the interface, select the positions with odd or even input data indices. The pseudo code is as follows:

```python
def asc_int322int64(dst, src, mask):
    for i in range(32):
        if mask[2 * i]:                  # Even-index positions participate in the computation
            dst[i] = int64(src[2 * i])   # int32 -> int64 widening conversion
        else:
            dst[i] = 0                   # Not selected by mask, set to zero

def asc_int322int64_v2(dst, src, mask):
    for i in range(32):
        if mask[2 * i + 1]:              # Odd-index positions participate in the computation
            dst[i] = int64(src[2 * i + 1])
        else:
            dst[i] = 0
```

### Migration Instructions

**Table 1** Correspondence between legacy and new position parameters

| Legacy prototype position suffix | New prototype position parameter |
| --- | --- |
| No position suffix | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

During migration, remove the position suffix from the function name and pass the corresponding position constant as the fourth parameter. For the new prototype, see [asc_int322int64](../reg_compute/reg_convert/asc_int322int64.md).

## Prototype

```c
// Placeholder form
__simd_callee__ inline void asc_int322int64<position_mode>(vector_int64_t& dst,
                                                           vector_int32_t src,
                                                           vector_bool mask)
```

**The placeholders are described as follows:**

- `<position_mode>` indicates the position from which `src` is read. When it is empty, elements at even-indexed positions of `src` are read; when its value is `_v2`, elements at odd-indexed positions of `src` are read.

### Prototype Example

```c
// Position where the data read index is even
__simd_callee__ inline void asc_int322int64(vector_int64_t& dst,
                                            vector_int32_t src,
                                            vector_bool mask)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description                                                                                                               |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst  | Output      | Destination operand (vector data register).                                                                                     |
| src  | Input      | Source operand (vector data register).                                                                                       |
| mask | Input      | Mask register, used to control whether each element participates in the computation. When the bit corresponding to an element in `mask` is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- The data types of `src` and `dst` must match the function prototype.
- When a `mask` bit is 0, the corresponding element of `dst` is set to 0.

## Examples

```cpp
__simd_vf__ inline void int322int64_vf(__ubuf__ int32_t* src_addr, __ubuf__ int64_t* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_int32_t src;
    vector_int64_t dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b32(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_int322int64(dst, src, mask);
        asc_storealign_pack_postupdate(dst_addr, dst, one_block_size, mask);
    }
}
```
