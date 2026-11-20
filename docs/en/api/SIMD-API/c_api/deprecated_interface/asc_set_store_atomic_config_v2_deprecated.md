---
okftype: reference.api
description: "Deprecated API page: On the 3510 architecture, asc_set_store_atomic_config_v2 uses two 16-bit parameters, type and op, to set the atomic operation enable bit and the atomic operation type. type selects data types such as int32, half, float, and bfloat16, and op set to 2 indicates summation. For atomic addition, use asc_atomic_add instead."
tag:
  - roles: [configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md
    kind: replaced_by
    condition: "Release Notes 9.1.0 requires atomic addition operations to directly call asc_atomic_add"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.1.0 lists asc_set_store_atomic_config_v2 as deprecated in the deprecation table and requires scalar atomic addition to directly call asc_atomic_add. The page prototype carries the deprecated attribute. This is a deprecation rather than a removal, and the API remains available."
---
# asc_set_store_atomic_config_v2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:56:12.021Z pushedAt=2026-11-18T03:12:16.669Z nl=2 -->

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

The header file path is: `"c_api/atomic/scalar_atomic.h"`.

**This API is deprecated. For atomic addition operations, use [asc_atomic_add](../atomic/scalar_atomic/asc_atomic_add.md) instead.**

Set the value of the atomic operation enable bit and the atomic operation type.

## Prototype

```c
[[deprecated("NOTICE: asc_set_store_atomic_config_v2 is deprecated."
             "Please use asc_atomic_add instead for atomic add operation.")]]
__aicore__ inline void asc_set_store_atomic_config_v2(uint16_t type, uint16_t op)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| type | Input | Atomic operation enable bit.<br>1: Enables the atomic operation, with the data type int32_t.<br>4: Enables the atomic operation, with the data type half.<br>5: Enables the atomic operation, with the data type float.<br>7: Enables the atomic operation, with the data type bfloat16_t.<br>9: Enables the atomic operation, with the data type int16_t.<br>10: Enables the atomic operation, with the data type int8_t.<br>Other values have no specific meaning. |
| op | Input | Atomic operation type.<br>2: Summation operation.<br>Other values have no specific meaning. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```c
uint16_t type = 1;
uint16_t op = 2;
asc_set_store_atomic_config_v2(type, op);
```
