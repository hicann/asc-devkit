---
okftype: reference.api
description: "Reads the current saturation control strategy and returns the enum value. It is commonly used to save the original configuration before temporarily switching strategies or to perform readback verification with the setting API."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_override_strategy.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_strategy.md
    kind: related
version: ">=9.2.0"
version_changes: []
---
# asc_get_saturation_strategy

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.472308+00:00 pushedAt=2026-11-16T02:10:57.590Z nl=2 -->

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

The header file path is `"c_api/spr/spr.h"`.

Reads the current saturation control strategy. Global saturation configuration is enabled by default:

- When the current configuration is per-instruction/API path, this API returns `asc_override_strategy::USE_API`.
- When the current configuration is global saturation, this API returns `asc_override_strategy::USE_GLOBAL`.

Commonly used to save the original configuration before temporarily switching strategies, or to perform readback verification together with [asc_set_saturation_strategy](asc_set_saturation_strategy.md).

## Prototype

```c
__aicore__ inline asc_override_strategy asc_get_saturation_strategy()
```

## Parameters

None

## Return Value

Current saturation control strategy, of type [asc_override_strategy](../defs/enum/asc_override_strategy.md).

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

Scenario: dynamic quantization, where overflow must be saturated to ±MAX instead of truncated. Set the strategy to `USE_GLOBAL` and enable CAST saturation first.

```cpp
// Outside SIMD_VF: save and switch the saturation strategy (before the dynamic quantization section starts).
asc_override_strategy old_strategy = asc_get_saturation_strategy();
bool old_cast_sat = asc_get_saturation_flag(asc_saturation_mode::CAST);
asc_set_saturation_strategy(asc_override_strategy::USE_GLOBAL);
asc_set_saturation_flag(asc_saturation_mode::CAST, true);

asc_copy_gm2ub_align(act_ub, act_gm, act_burst_bytes);
asc_copy_gm2ub_align(bias_ub, bias_gm, bias_burst_bytes);
asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

// Inside the vector computation loop: after activation + bias addition, float → int32 (global CAST saturation takes effect).

// Outside SIMD_VF: restore the configuration from before entering this section.
asc_set_saturation_flag(asc_saturation_mode::CAST, old_cast_sat);
asc_set_saturation_strategy(old_strategy);
```
