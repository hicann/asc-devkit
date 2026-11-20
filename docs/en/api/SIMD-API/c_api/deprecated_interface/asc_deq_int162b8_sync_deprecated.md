---
okftype: reference.api
description: "Deprecated int16_t dequantization synchronous interface. It converts int16_t to int8_t or uint8_t based on the scale and offset configured by asc_set_deq_scale, and stores the result in the high half-block or low half-block of each DataBlock. The replacement is the corresponding asynchronous interface used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [datablock_granularity, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the asynchronous interface, you must call asc_sync to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int162b8.md
    kind: replaced_by
    condition: "The h and l asynchronous interfaces replace the synchronous interface on this page."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: requires
    condition: "The quantization scale and offset must be set before the call."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks all 4 overloads of the two synchronous interfaces h and l with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_deq_int162b8_h/l and asc_sync); the CANN_9_2_0 Release Notes deprecation table lists this type of *_sync composite interface; deprecation does not close the available scope, and Atlas A2 and Atlas A3 products are still supported."
---
# asc_deq_int162b8_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:44:33.619Z pushedAt=2026-11-18T03:12:16.461Z nl=2 -->

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

**The `asc_deq_int162b8_h_sync` and `asc_deq_int162b8_l_sync` interfaces are deprecated. Use the [corresponding asynchronous interface](../vector_compute/vector_fused/asc_deq_int162b8.md) together with the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) as a replacement.**

Converts int16_t data to int8_t or uint8_t and stores the data in the high half-block or low half-block of each DataBlock. Before using this interface, call [asc_set_deq_scale](../vector_compute/vector_fused/asc_set_deq_scale.md) to set the quantization parameters.

- asc_deq_int162b8_h: Stores the data in the high half-block of each DataBlock.
- asc_deq_int162b8_l: Stores the data in the low half-block of each DataBlock.

As shown in the following figure:
![](../../../figures/asc_deq_int162b8_halfblock.png)

The calculation formula is as follows:

$$
dst_i = (src_i * scale) + offset
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the computation.
__ubuf__ int16_t src[total_length];
__ubuf__ int8_t dst[total_length];
float scale = 1.0;        // The quantization parameter is 1.
int16_t offset = 0;       // No offset.
bool sign_mode = true;    // The quantization result is signed (dst is of the int8_t type).
asc_set_deq_scale(scale, offset, sign_mode);    // The calculation formula is dst = src.
asc_deq_int162b8_h_sync(dst, src, total_length);    // Convert src to the int8_t type and store it in the high half-block of each DataBlock in dst.
```
