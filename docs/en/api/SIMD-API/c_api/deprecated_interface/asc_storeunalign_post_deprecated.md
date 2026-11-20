---
okftype: reference.api
description: "Deprecated API page: asc_storeunalign_post moves the tail block temporarily stored in the unaligned register by asc_storeunalign_postupdate to a non-32-byte-aligned address in UB. dst does not need 32-byte alignment but must be aligned by element width. It is recommended to use asc_squeeze_and_storeunalign_finalize instead."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The unaligned register and address register are defined according to their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/deprecated_interface/asc_storeunalign_postupdate_deprecated.md
    kind: requires
    condition: "This API must be called first to move out the main block and temporarily store the tail block in the unaligned register, and both must use the same unaligned register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires unifying the valid element filtering and unaligned continuous store-out processes"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_storeunalign_post and some old asc_storeunalign_postupdate overloads as deprecated, replaced by asc_squeeze_and_storeunalign and finalize or the current same-name B16/B32 overloads; the APIs are not removed."
---
# asc_storeunalign_post (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:04:46.918Z pushedAt=2026-11-18T03:12:16.682Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/store/storeunalign.h"`.

**This API is deprecated. Use [asc_squeeze_and_storeunalign_finalize](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md) to implement this function instead.**

A reg compute data transfer API, applicable to the tail block scenario where data is continuously moved from a non-32B-aligned start address in a vector data register to the Unified Buffer (UB).

You need to call one of the following APIs before calling this API.
- The [asc_storeunalign_postupdate](asc_storeunalign_postupdate_deprecated.md) API.

The transfer principle is as follows:
Let the start address of the destination operand be dst_start, the end address be dst_end, and the number of tail block elements be unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T).
Then the transferred data is divided into two parts:
- The 32B-aligned main block part UB[dst_start, dst_end - unalign_count * sizeof(T)].
- The non-32B-aligned tail block part UB[dst_end - unalign_count * sizeof(T), dst_end].
When the store_unalign or store_unalign_postupdate API is executed, the main block is moved out to the UB, and the tail block is temporarily stored in the unaligned register [0, unalign_count].
When this API is executed, the tail block is moved from the unaligned register to the UB.

## Prototype

```cpp
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int8_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int4b_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ half* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ bfloat16_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int32_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint32_t* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ float* dst, vector_store_unalign src)
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ int64_t* dst, vector_store_unalign src)
```

## Parameters

- No offset specified

    **Table 1** Parameter description

    | Parameter | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Start address of the destination operand (vector). |
    | src | Input | Unaligned register used to store unaligned data, with a length of 32B. |

For details about the unaligned register and address register, see [reg Data Type Definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The dst in this API does not need to be 32B aligned, but dst of data type T needs to be sizeof(T) Byte aligned.
- Before calling this API, you need to call the [asc_storeunalign_postupdate](asc_storeunalign_postupdate_deprecated.md) API, and the unaligned register of this API must be consistent with that of the preceding API.
- You need to ensure that the destination operand address plus the offset address corresponding to offset equals the end address of the data movement. For details, see the calling example.

## Examples

- No offset specified

    ```cpp
    // The dst address is 8, which is not 32B aligned. The range is [8:520].
    __ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```
