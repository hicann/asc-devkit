---
okftype: reference.api
description: "Deprecated API page: asc_storeunalign_postupdate moves vector or mask register data continuously out to a UB non-32-byte-aligned address and temporarily stores the tail block in the unaligned register, automatically updating the destination address on each call. Use asc_squeeze_and_storeunalign or the current B16/B32 overloads instead."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [post_update, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of the vector data register, unaligned register, and address register follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/deprecated_interface/asc_storeunalign_post_deprecated.md
    kind: related
    condition: "This API moves out the main block and temporarily stores the tail block, which is written out to UB by that API"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires switching to this API family for unified valid element filtering and unaligned continuous move-out"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 deprecation table lists some old overloads of asc_storeunalign_postupdate (including the prototype without a specified offset and the mask register b8 prototype) as deprecated, replaced by the asc_squeeze_and_storeunalign series or the current same-name B16/B32 overloads; the API is not removed."
---
# asc_storeunalign_postupdate (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:05:44.282Z pushedAt=2026-11-18T03:12:16.683Z nl=2 -->

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

**The prototype of this API with unspecified storage offset is deprecated. Use [asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) instead to implement this function. The b8 data type prototype of the mask register move-out mode is deprecated. Use the b16 or b32 data type prototype instead.**

A reg compute data move API, applicable to the scenario of continuously moving data out from a vector data register or mask register to a Unified Buffer (UB) starting address that is not 32B aligned.

Each call to this API updates the address of the destination operand in the UB.

## Prototype

- Unspecified storage offset

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1)
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1)
    ```

- Mask register move-out mode (b8 data type)

    ```cpp
    __simd_callee__ inline void asc_storeunalign_postupdate(__ubuf__ uint8_t*& dst, vector_store_unalign& src0, vector_bool src1)
    ```

## Parameters

- Unspecified storage offset

    **Table 1** Parameter description

    | Parameter  | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Output | Start address of the destination operand (vector). |
    | src0 | Input/Output | Unaligned register, used to store unaligned data, with a length of 32B. |
    | src1 | Input | Source operand (vector data register or mask register). |

- Mask register move-out mode (b8 data type)

    **Table 2** Parameter Description

    | Parameter | Input/Output | Description |
    | :----- | :------- | :------- |
    | dst | Input/Output | Starting address of the destination operand in UB, with the data type `uint8_t`. After the move-out, `dst` is automatically updated to the end address of this move-out. |
    | src0 | Input/Output | Unaligned register used to store unaligned data, with a length of 32B. |
    | src1 | Input | Source operand (mask register), with the type `vector_bool`. |

For details about the vector data register, unaligned register, and address register, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The dst in this API does not need to be 32B aligned.
- This API should be used together with [asc_storeunalign_post](asc_storeunalign_post_deprecated.md).
- The b8 data type prototype of the mask register move-out mode is deprecated. Use the b16 or b32 data type prototype instead.

## Examples

- Unspecified storage offset

    ```cpp
    // dst is a UB address, with the starting address 8, not 32B aligned
    vector_store_unalign ureg;
    vector_uint32_t src;
    asc_storeunalign_postupdate(dst, ureg, src);
    asc_storeunalign_post(dst, ureg);
    ```
