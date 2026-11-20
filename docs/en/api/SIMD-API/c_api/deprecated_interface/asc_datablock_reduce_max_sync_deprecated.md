---
okftype: reference.api
description: "Deprecated synchronous DataBlock reduce max interface that takes the maximum of elements within each 32-byte data block. The alternative is asc_datablock_reduce_max plus asc_sync."
tag:
  - roles: [compute]
  - operations: [maximum, reduction]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "synchronous part of the alternative"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_max.md
    kind: replaced_by
    condition: "alternative asynchronous interface declared in the body"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.2.0, the include marker ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_datablock_reduce_max and asc_sync); the declaration is retained, and it belongs to deprecated rather than removed."
---
# asc_datablock_reduce_max_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:41:54.786Z pushedAt=2026-11-18T03:12:16.456Z nl=2 -->

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

**The `asc_datablock_reduce_max_sync` interface is deprecated. Use the [corresponding asynchronous interface](../vector_compute/vector_reduce/asc_datablock_reduce_max.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs the reduce max operation on elements within a data block.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector). |
| src     | Input     | Start address of the source operand (vector). |
| count   | Input     | Number of elements involved in continuous computation.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 16;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// The synchronous interface uses count to specify the total number of elements involved in the computation, so there is no need to set the mask manually.
asc_datablock_reduce_max_sync(dst, src, src_length);
```

Result example:

```
Input data src: [1 2 3 ... 16 17 ... 32 ... 225... 256]
Output data dst: [16 32 ... 256]
```
