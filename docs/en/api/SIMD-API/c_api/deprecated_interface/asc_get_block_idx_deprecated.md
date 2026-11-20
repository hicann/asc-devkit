---
okftype: reference.api
description: "Deprecated core index query API that returns the current logical core index. For migration, in pure Vector or Cube scenarios, directly use the built-in variable block_idx; in Mix scenarios, calculate it as block_idx multiplied by the number of sub-blocks plus the sub-block ID. The header file is c_api/utils/sys_var.h."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md
    kind: related
    condition: "Provides the ID of the current Vector Core when calculating the index by logical position in Mix scenarios"
  - path: docs/zh/api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_num.md
    kind: related
    condition: "Provides the number of sub-blocks when calculating the index by logical position in Mix scenarios"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: replaced_by
    condition: "In non-Mix scenarios, the built-in variable block_idx can be used directly to obtain the same return value as the original API"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The declaration in include/c_api/utils/sys_var.h is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", block_idx and asc_get_sub_block_num and asc_get_sub_block_id), and this API is also listed in the deprecated APIs table of the CANN 9.2.0 Release Notes. The declaration is still retained in 9.3.0-beta.1, which means it is deprecated rather than removed, with no upper version limit."
---
# asc_get_block_idx (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:20:28.220Z pushedAt=2026-11-18T03:12:16.515Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

The header file path is: `"c_api/utils/sys_var.h"`.

**This API is deprecated. Use the [built-in variable block_idx](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables) in combination with [asc_get_sub_block_num](../utils/sys_var/asc_get_sub_block_num.md) and [asc_get_sub_block_id](../utils/sys_var/asc_get_sub_block_id.md) instead.**

Obtains the index of the current running core (logical core). When replacing it, calculate based on the logical position:

```cpp
int64_t blockIdx = block_idx * asc_get_sub_block_num() + asc_get_sub_block_id();  // Replaces asc_get_block_idx()
__gm__ float* xGm = x + blockIdx * blockLength;
```

### Migration Guide

- In non-Mix scenarios, you can directly use the built-in variable `block_idx`, whose value equals the return value of `asc_get_block_idx()`.
- In Mix scenarios (using the `__mix__` function execution space qualifier), the built-in variable `block_idx` only identifies the position of the current combination in the entire grid. The logical core index must be calculated together with sub-block information: `logic_idx = block_idx * sub_block_num + sub_block_id`, that is, `block_idx * asc_get_sub_block_num() + asc_get_sub_block_id()`. The value of this expression is consistent with the return value of the original API.
- Before use, refer to [Built-in Variable Description](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables) to confirm the applicable scenarios.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_block_idx()
```

## Parameters

None

## Return Value

Index of the currently running core.

## Pipeline Type

`PIPE_S`

## Constraints

None

## Examples

```cpp
int64_t blockIdx = block_idx * asc_get_sub_block_num() + asc_get_sub_block_id();  // Obtain the index of the currently running core, replacing asc_get_block_idx().
```
