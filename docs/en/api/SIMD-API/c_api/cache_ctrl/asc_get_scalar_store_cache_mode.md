---
okftype: reference.api
description: "Reads the L2 cache management policy of the current scalar store path and returns the asc_store_l2_cache_mode enumeration value, which is set by asc_set_scalar_cache_mode. It is commonly used to save the original configuration before temporarily switching the policy or to perform read-back verification after setting."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "Reads back the scalar store policy written by this setting interface"
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md
    kind: related
    condition: "The return value type is this enumeration"
version: ">=9.2.0"
version_changes: []
---
# asc_get_scalar_store_cache_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:52:24.054Z pushedAt=2026-11-18T08:15:34.198Z nl=2 -->

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

The header file path is: `"c_api/cache_ctrl/cache_ctrl.h"`.

Reads the L2 cache management policy of the current scalar store path and returns the [asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md) enumeration value. This value can be set through the [asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md) interface. For detailed description of the policy, refer to that interface.

It is commonly used to save the original configuration before temporarily switching the policy, or to perform read-back verification after setting.

## Prototype

```c
__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode()
```

## Parameters

None

## Return Value

The L2 cache management policy of the current scalar store path, of type [asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md).

## Pipeline Type

PIPE_S

## Constraints

- This interface only reflects the policy on the scalar store side, and is independent of the scalar load side and the `l2_cache_mode` in the vector transfer interface parameters.

## Examples

This interface is used to save the current policy of the scalar store path. For a complete call example, refer to [Scenario B of asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md#asc_set_scalar_cache_mode_scenario_b).
