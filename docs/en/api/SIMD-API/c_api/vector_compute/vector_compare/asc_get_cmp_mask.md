---
okftype: reference.api
description: "Retrieves the comparison result of a Compare operation from the register and writes it to the UB destination address. Must be used together with a comparison operation. Each element comparison result occupies 1 bit."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "Address overlap and general constraint scope are consistent with the same family."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_eq.md
    kind: related
    condition: "The description requires use with the Compare operation, with asc_eq as the family representative."
version: ">=8.5.0"
version_changes: []
---
# asc_get_cmp_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.730656+00:00 pushedAt=2026-11-16T02:10:58.364Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_compare.h"`.

This API is used to obtain the comparison result of the Compare operation.

## Prototype

```cpp
__aicore__ inline void asc_get_cmp_mask(__ubuf__ void* dst)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Address for storing the comparison result.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

It must be used together with the Compare operation.

## Examples

```cpp
constexpr uint32_t total_length = 128;
__ubuf__ int8_t dst[total_length];
...     // Perform the Compare operation.
asc_get_cmp_mask(dst);
```