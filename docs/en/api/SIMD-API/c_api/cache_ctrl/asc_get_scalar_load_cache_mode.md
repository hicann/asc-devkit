---
okftype: reference.api
description: "Reads the L2 Cache management policy of the current scalar load path and returns the asc_load_l2_cache_mode enum value. This value is set by asc_set_scalar_cache_mode and is commonly used to save the original configuration before temporarily switching the policy or to perform readback verification after setting."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_set_scalar_cache_mode.md
    kind: related
    condition: "The readback is the scalar load policy written by this setting interface."
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: related
    condition: "The return value type is this enum."
version: ">=9.2.0"
version_changes: []
---
# asc_get_scalar_load_cache_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:50:56.877Z pushedAt=2026-11-18T08:15:34.196Z nl=2 -->

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

The header file path is `"c_api/cache_ctrl/cache_ctrl.h"`.

Reads the L2 Cache management policy of the current scalar load path and returns the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) enumeration value. This value can be set through the [asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md) interface. For details about the policy, see that interface.

It is commonly used to save the original configuration before temporarily switching the policy, or to perform readback verification after setting.

## Prototype

```c
__aicore__ inline asc_load_l2_cache_mode asc_get_scalar_load_cache_mode()
```

## Parameters

None

## Return Value

The L2 Cache management policy of the current scalar load path, of the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) type.

## Pipeline Type

PIPE_S

## Constraints

This interface only reflects the policy on the scalar load side and is unrelated to the scalar store side and the `l2_cache_mode` in the vector copy interface parameters.

## Examples

This interface is used to save the current policy of the scalar load path. For a complete call example, see [scenario A of asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md#asc_set_scalar_cache_mode_scenario_a).
