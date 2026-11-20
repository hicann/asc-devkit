---
okftype: reference.api
description: "Deprecated int32_t dequantization synchronous interface. It uses the scale set by asc_set_deq_scale to quantize int32_t to half element-wise and waits for the vector pipeline to complete. The replacement is asc_deq_int322half used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronous interface, you must call asc_sync to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_deq_int322half.md
    kind: replaced_by
    condition: "The non-synchronous interface replaces the synchronous interface on this page"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: requires
    condition: "Must be used with asc_set_deq_scale in non-vector mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks ASC_DEPRECATED(9.2.0, 2027/09/07, asc_deq_int322half and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this type of *_sync composite interface; this interface is retained after replacement and has not been removed."
---
# asc_deq_int322half_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:45:49.326Z pushedAt=2026-11-18T03:12:16.462Z nl=2 -->

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

**The `asc_deq_int322half_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_fused/asc_deq_int322half.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Quantizes the input int32_t data element-wise and converts it to the half type. The calculation formula is as follows:
$$
dst_i = Cast(src_i * scale)
$$
where scale can be set through [asc_set_deq_scale](../vector_compute/vector_fused/asc_set_deq_scale.md).

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_deq_int322half_sync(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count)
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

- Must be used together with the non-vector mode of the [asc_set_deq_scale](../vector_compute/vector_fused/asc_set_deq_scale.md) API, which is used to set the quantization parameters.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the data length involved in the computation.
__ubuf__ int32_t src[total_length];
__ubuf__ half dst[total_length];
asc_set_deq_scale(2.0);
…… // Data transfer and synchronization operations.
asc_deq_int322half_sync(dst, src, total_length);
```
