---
okftype: reference.api
description: "Deprecated core count query API that returns the number of cores configured for the current task. During migration, directly use the built-in variable block_num in the code; both return the same value. The header file is c_api/utils/sys_var.h."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: replaced_by
    condition: "The value of the built-in variable block_num is consistent with the return value of the original API and can be used as a direct replacement"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The declaration in include/c_api/utils/sys_var.h is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", block_num). The CANN 9.2.0 Release Notes API deprecation table lists asc_get_block_num and asc_get_block_idx together as APIs to be replaced by built-in variables. The declaration is still retained in 9.3.0-beta.1, indicating deprecation rather than removal, so no upper version limit is set."
---
# asc_get_block_num (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:21:53.632Z pushedAt=2026-11-18T03:12:16.516Z nl=2 -->

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

**This API is deprecated. Use the [built-in variable block_num](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables) instead.**

Obtains the number of cores configured for the current task, which is used for multi-core logic control within the code. It is recommended to directly use the built-in variable `block_num` in the code:

```cpp
uint32_t blockLength = C_API_TOTAL_LENGTH / block_num;  // Replace asc_get_block_num().
```

### Migration Instructions

The value of the built-in variable `block_num` is the number of cores configured for the current task, which is consistent with the return value of the original API. You can use it directly as a replacement. Before use, refer to [Built-in Variable Description](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#built-in-variables) to confirm the applicable scenarios.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_block_num()
```

## Parameters

None

## Return Value

Number of cores configured for the current task.

## Pipeline Type

`PIPE_S`

## Constraints

None

## Examples

```cpp
uint32_t blockLength = C_API_TOTAL_LENGTH / block_num;  // Obtain the number of cores configured for the current task, replacing asc_get_block_num().
```