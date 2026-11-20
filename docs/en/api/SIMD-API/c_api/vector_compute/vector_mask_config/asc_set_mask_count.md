---
okftype: reference.api
description: "Configuration API that sets the Mask mode to Counter mode. In this mode, there is no need to perceive the iteration count or process unaligned tail blocks; the actual iteration count is automatically inferred by the Vector Compute unit. APIs for computation on the first n elements set this internally and automatically. Must be used together with asc_set_vector_mask, with pipeline type PIPE_S."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_vector_mask.md
    kind: related
    condition: "The Counter mode set by this API must be used together with the mask value setting API."
version: ">=8.5.0"
version_changes: []
---
# asc_set_mask_count

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.924577+00:00 pushedAt=2026-11-16T02:10:59.052Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_mask_config.h"`.

Sets the mask mode to Counter mode. In this mode, developers do not need to be aware of the number of iterations or handle unaligned tail blocks. They can directly pass in the number of data elements to be computed, and the actual number of iterations is automatically inferred by the Vector computing unit. This mode is automatically set inside the APIs for computing the first n data elements, so it does not need to be set manually.

## Prototype

```cpp
__aicore__ inline void asc_set_mask_count()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

This API must be used together with [asc_set_vector_mask](asc_set_vector_mask.md).

## Examples

```cpp
asc_set_mask_count();
asc_set_vector_mask(0, static_cast<uint64_t>(64)); // Set the first 64 elements to participate in the computation.
... // Perform the computation.
asc_set_mask_norm();    // Restore Normal mode. This step can be configured as needed.
```