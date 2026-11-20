---
okftype: reference.api
description: "Query API that obtains the number of remaining elements after an asc_reduce operation. It has no parameters, returns int64_t, must be called after asc_reduce execution completes and synchronization, and has pipeline type PIPE_S."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_reduce.md
    kind: related
    condition: "Reads the number of remaining elements after this data collection API executes"
version: ">=8.5.0"
version_changes: []
---
# asc_get_rsvd_count

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.974057+00:00 pushedAt=2026-11-16T02:10:59.229Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_reduce.h"`.

This API is used to obtain the number of remaining elements after the [asc_reduce](asc_reduce.md) operation is executed.

## Prototype

```cpp
  __aicore__ inline int64_t asc_get_rsvd_count()
```

## Parameters

None

## Return Value

Returns the number of remaining elements after the [asc_reduce](asc_reduce.md) operation is executed.

## Pipeline Type

PIPE_S

## Constraints

- It must be used together with the [asc_reduce](asc_reduce.md) operation.
- Use a synchronization operation to ensure that [asc_reduce](asc_reduce.md) has completed before calling this API to obtain the result.

## Examples

```cpp
// Initialize dst, src0, src1, and total_length (the data length involved in the computation).
int total_length = 256;
__ubuf__ uint32_t src0[total_length];
__ubuf__ uint32_t src1[total_length];
__ubuf__ uint32_t dst[total_length];
asc_reduce(dst, src0, src1, total_length);
asc_sync_notify(PIPE_V, PIPE_S, 0); // Set wait and synchronization signals.
asc_sync_wait(PIPE_V, PIPE_S, 0);
int64_t result = asc_get_rsvd_count();  // Get the result.
```
