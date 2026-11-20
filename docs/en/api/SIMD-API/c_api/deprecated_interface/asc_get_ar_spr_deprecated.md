---
okftype: reference.api
description: "Deprecated special register read interface that takes no input parameters and returns an int64_t special register value. It runs on PIPE_S and is supported only on Ascend 950PR&950DT. Use asc_get_squeeze_status instead."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_squeeze_status.md
    kind: replaced_by
    condition: "The squeeze status read capability that replaces this interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The declaration in include/c_api/spr/spr.h is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_get_squeeze_status). The CANN 9.2.0 Release Notes interface deprecation table also lists asc_get_ar_spr and recommends using asc_get_squeeze_status instead. The declaration is still retained in 9.3.0-beta.1, indicating deprecation rather than removal."
---
# asc_get_ar_spr (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:20:21.644Z pushedAt=2026-11-18T03:12:16.514Z nl=2 -->

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

The header file path is: `"c_api/spr/spr.h"`.

**This API is deprecated. Use [asc_get_squeeze_status](../spr/asc_get_squeeze_status.md) to implement this function instead.**

Reads the value of the specified special register.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_ar_spr()
```

## Parameters

None

## Return Value

Returns the value in the int64_t special register.

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

None
