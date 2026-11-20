---
okftype: reference.api
description: "Deprecated vector duplicate synchronous interface that copies and fills a half, int16_t, uint16_t, bfloat16_t, float, int32_t, or uint32_t scalar into count consecutive elements of the destination vector and waits for the pipeline. The replacement is asc_duplicate combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [broadcast]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronous interface, call asc_sync to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_broadcast/asc_duplicate.md
    kind: replaced_by
    condition: "The non-synchronous duplicate interface replaces the synchronous interface on this page."
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header uniformly marks the 7 scalar type overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_duplicate and asc_sync); the CANN_9_2_0 Release Notes deprecation table lists this class of *_sync composite interfaces; the interface is retained and not removed."
---
# asc_duplicate_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:47:48.848Z pushedAt=2026-11-18T03:12:16.465Z nl=2 -->

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

**The `asc_duplicate_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_broadcast/asc_duplicate.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs a vector duplicate operation, copying and filling a scalar value into a vector.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_duplicate_sync(__ubuf__ half* dst, half src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ int16_t* dst, int16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ float* dst, float src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ int32_t* dst, int32_t src, uint32_t count)
    __aicore__ inline void asc_duplicate_sync(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Source operand (scalar). |
| count | Input | Number of elements involved in continuous duplication. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of dst must be 32-byte aligned.

## Examples

```cpp
constexpr uint32_t dst_length = 128;
__ubuf__ uint16_t dst[dst_length];
uint16_t val = 18;
asc_duplicate_sync(dst, val, dst_length);
```

Result example:

```
Output data dst: [18 18 18 ... 18]
```
