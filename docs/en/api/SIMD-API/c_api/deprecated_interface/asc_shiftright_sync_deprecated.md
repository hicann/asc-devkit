---
okftype: reference.api
description: "Deprecated interface page: On the 2201 architecture, asc_shiftright_sync and asc_shiftright_round_sync right shift vector elements by value bits. Signed types perform arithmetic right shift, and unsigned types perform logical right shift. The round form additionally rounds. It is recommended to use the corresponding non-sync interface plus asc_sync instead."
tag:
  - roles: [compute]
  - operations: [bitwise_shift]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The address overlap constraint follows its generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles synchronization waiting in the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_logic/asc_shiftright.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-sync interface and explicitly call asc_sync"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks ASC_DEPRECATED(9.2.0) before the asc_shiftright_sync declaration, and Release Notes 9.2.0 lists the _sync composite interfaces as deprecated as a whole; the interface has not been removed."
---
# asc_shiftright_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:57:51.436Z pushedAt=2026-11-18T03:12:16.672Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_shiftright_sync` and `asc_shiftright_round_sync` interfaces are deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_logic/asc_shiftright.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs a right shift on each element in src, with the shift amount being value.

- For int16_t/int32_t, performs an arithmetic right shift (the lowest bit is removed, and the highest bit is filled with the sign bit).
- For uint16_t/uint32_t, performs a logical right shift (the lowest bit is removed, and the highest bit is filled with 0).

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_shiftright_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
    __aicore__ inline void asc_shiftright_round_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| value | Input | Source operand (scalar), indicating the number of bits to shift right. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- value does not support being set to a negative number.
- The value of value cannot exceed the bit width of the src data type.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the length of data involved in the computation
int32_t value = 1;
__ubuf__ int16_t src[total_length];
__ubuf__ int16_t dst[total_length];
asc_shiftright_sync(dst, src, value, total_length);    // Perform an arithmetic right shift by one bit on each element in src, and store the result in dst.
// src[0] = 0b1010, dst[0] = 0b0101

asc_shiftright_round_sync(dst, src, value, total_length);    // Perform an arithmetic right shift by one bit on each element in src and round the result, then store it in dst.
// src[0] = 0b1010, dst[0] = 0b0101, the last bit rounded off is 0, so no carry occurs
// src[0] = 0b1001, dst[0] = 0b0101, the last bit rounded off is 1, so a carry occurs
```
